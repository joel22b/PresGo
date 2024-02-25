from collections import deque, namedtuple
from enum import Enum
import math
import threading
import time
import tkinter

WINDOW_TITLE = 'PresGo GUI'
WINDOW_WIDTH_PX = 800
WINDOW_HEIGHT_PX = 800
FONT = 'Arial'
FONT_SIZE = 24
FONT_WEIGHT = 'bold'
STATUS_TEXT_TEMPLATE = 'System Status: '
TIME_IN_SUCCESS_OR_FAILURE_STATE_S = 1.5

class TkinterGUI:
  def __init__(self, close_event):
    # setup graceful shutdown
    self.close_event = close_event
    # initialize system status, state and state queue handling thread
    self.system_status = SystemStatus.RUNNING
    self.state = State.WAITING
    self.state_queue = deque()
    self.state_thread = threading.Thread(target=self.process_state_queue, daemon=True)
    self.state_thread.start()
    # initialize gui
    self.root = tkinter.Tk()
    self.root.title(WINDOW_TITLE)
    self.canvas = tkinter.Canvas(self.root, width=WINDOW_WIDTH_PX, height=WINDOW_HEIGHT_PX, bg=self.state.value.color)
    self.canvas.pack(fill='both', expand=True)
    self.gui_main_text = self.canvas.create_text(math.ceil(WINDOW_WIDTH_PX/2), math.ceil(WINDOW_HEIGHT_PX/2), text=self.state.value.default_message, font=(FONT, FONT_SIZE, FONT_WEIGHT), anchor='center')
    self.gui_status_text = self.canvas.create_text(WINDOW_WIDTH_PX-FONT_SIZE/2, WINDOW_HEIGHT_PX-FONT_SIZE/2, text=self.system_status.value.default_message, font=(FONT, FONT_SIZE, FONT_WEIGHT), fill=self.system_status.value.color, anchor='se')
    self.gui_status_text_template = self.canvas.create_text(self.canvas.bbox(self.gui_status_text)[0], WINDOW_HEIGHT_PX-FONT_SIZE/2, text=STATUS_TEXT_TEMPLATE, font=(FONT, FONT_SIZE, FONT_WEIGHT), fill='black', anchor='se')  
    self.root.after(100, self.check_close_event)

  def start_main_loop(self):
    self.root.mainloop()

  def check_close_event(self):
    if self.close_event.is_set():
      self.root.destroy()
    else:
      self.root.after(100, self.check_close_event)

  def process_state_queue(self):
    while not self.close_event.is_set():
      if not self.state_queue:
        continue
      state, display_text = None, None
      # when in 'VALIDATING' state, ensure the result of the validation is the next state selected
      if self.state == State.VALIDATING:
        found_validation_result = False
        for state_with_message in list(self.state_queue):
          # only want to transition to a success or failure failure state related to the validation, not a failure state caused by someone walking through with no tag tag
          if (state_with_message.state == State.SUCCESS or state_with_message.state == State.FAILURE) and state_with_message.message == state_with_message.state.value.default_message:
            found_validation_result = True
            state, display_text = state_with_message
            self.state_queue.remove(state_with_message)
            break
        if not found_validation_result:
          continue
      else:
        state, display_text  = self.state_queue.popleft()
      self.state = state
      background = state.value.color
      self.canvas.config(bg=background)
      self.canvas.itemconfig(self.gui_main_text, text=display_text)
      if state == State.SUCCESS or state == State.FAILURE:
        time.sleep(TIME_IN_SUCCESS_OR_FAILURE_STATE_S)

  def set_state(self, state, display_text=None):
    display_text = display_text if display_text is not None else state.value.default_message
    self.state_queue.append(StateWithMessage(state=state, message=display_text))
    # add waiting state to the queue after failure or success status, to be shown after TIME_IN_SUCCESS_OR_FAILURE_STATE_S seconds
    if state == State.SUCCESS or state == State.FAILURE:
      self.state_queue.append(StateWithMessage(state=State.WAITING, message=State.WAITING.value.default_message))

  def set_system_status(self, system_status):
    self.system_status = system_status
    self.canvas.itemconfig(self.gui_status_text, text=system_status.value.default_message)

StateWithMessage = namedtuple('StateWithMessage', ['state', 'message'])
ColorWithDefaultMessage = namedtuple('ColorWithDefaultMessage', ['color', 'default_message'])

class State(Enum):
  WAITING = ColorWithDefaultMessage(color='grey', default_message='Waiting for passengers...')
  VALIDATING = ColorWithDefaultMessage(color='yellow', default_message='Attempting to process payment...')
  SUCCESS = ColorWithDefaultMessage(color='green', default_message='Payment completed successfully.')
  FAILURE = ColorWithDefaultMessage(color='red', default_message='Payment was not successful.')

class SystemStatus(Enum):
  RUNNING = ColorWithDefaultMessage(color='green', default_message='Running')
  VALIDATING = ColorWithDefaultMessage(color='red', default_message='Error')
