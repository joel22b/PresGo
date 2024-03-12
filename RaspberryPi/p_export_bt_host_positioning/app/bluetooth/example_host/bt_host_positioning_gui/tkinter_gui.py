from collections import deque, namedtuple
from datetime import datetime
from enum import Enum
import os
import threading
from threading import Lock
import time
import tkinter

WINDOW_TITLE = 'PresGo GUI'
WINDOW_WIDTH_PX = 800
WINDOW_HEIGHT_PX = 455
WINDOW_PADDING_PX = 12
FONT = 'Arial'
FONT_SIZE = 18
FONT_WEIGHT = 'bold'
PERSON_COUNTER_TEXT_TEMPLATE = 'Bus Occupancy: '
STATUS_TEXT_TEMPLATE = 'System Status: '
RESET_BUTTON_TEXT = 'RESET'
VALIDATION_UI_TIMEOUT_S = 7
TIME_IN_SUCCESS_OR_FAILURE_STATE_S = 2

ColorWithDefaultMessage = namedtuple('ColorWithDefaultMessage', ['color', 'default_message'])
StateWithMessage = namedtuple('StateWithMessage', ['state', 'message'])

class SystemStatus(Enum):
  RUNNING = ColorWithDefaultMessage(color='green', default_message='Running')
  ERROR = ColorWithDefaultMessage(color='red', default_message='Error')

class State(Enum):
  WAITING = ColorWithDefaultMessage(color='grey', default_message='Waiting for passengers...')
  VALIDATING = ColorWithDefaultMessage(color='yellow', default_message='Attempting to process payment...')
  SUCCESS = ColorWithDefaultMessage(color='green', default_message='Payment successful. ')
  FAILURE = ColorWithDefaultMessage(color='red', default_message='Payment failed. ')

class TkinterGUI:
  def __init__(self, fob_processing):
    # setup graceful shutdown
    self.running = True
    # save reference of fob_processing to call its reset() function on press of the gui's reset button
    self.fob_processing = fob_processing
    # initialize person counter, system status, state and state queue handling thread
    self.lock = Lock()
    self.on_reset()
    self.state_thread = threading.Thread(target=self.process_state_queue, daemon=True)
    self.state_thread.start()
    # initialize gui
    self.root = tkinter.Tk()
    self.root.title(WINDOW_TITLE)
    self.canvas = tkinter.Canvas(self.root, width=WINDOW_WIDTH_PX, height=WINDOW_HEIGHT_PX, bg=self.state.value.color)
    self.canvas.pack(fill='both', expand=True)
    self.person_counter_text = self.canvas.create_text(WINDOW_PADDING_PX, WINDOW_HEIGHT_PX-WINDOW_PADDING_PX, text=self.get_person_counter_string(), font=(FONT, FONT_SIZE, FONT_WEIGHT), fill='black', anchor='sw')
    self.status_text = self.canvas.create_text(WINDOW_WIDTH_PX-WINDOW_PADDING_PX, WINDOW_HEIGHT_PX-WINDOW_PADDING_PX, text=self.system_status.value.default_message, font=(FONT, FONT_SIZE, FONT_WEIGHT), fill=self.system_status.value.color, anchor='se')
    self.status_template_text = self.canvas.create_text(self.canvas.bbox(self.status_text)[0], WINDOW_HEIGHT_PX-WINDOW_PADDING_PX, text=STATUS_TEXT_TEMPLATE, font=(FONT, FONT_SIZE, FONT_WEIGHT), fill='black', anchor='se')
    lower_text_bounding_box = self.canvas.bbox(self.person_counter_text)
    self.canvas.create_window(WINDOW_WIDTH_PX/2, WINDOW_HEIGHT_PX-6, window=tkinter.Button(self.root, text=RESET_BUTTON_TEXT, font=(FONT, FONT_SIZE, FONT_WEIGHT), activebackground=State.FAILURE.value.color, highlightthickness=0, borderwidth=0, command=self.initiate_reset), anchor='s')
    bottom_rectangle = self.canvas.create_rectangle(lower_text_bounding_box[0]-WINDOW_PADDING_PX, lower_text_bounding_box[1]-WINDOW_PADDING_PX, WINDOW_WIDTH_PX, lower_text_bounding_box[3]+WINDOW_PADDING_PX, fill='#3B3B3B')
    self.canvas.tag_lower(bottom_rectangle, self.person_counter_text)
    self.main_text = self.canvas.create_text(WINDOW_WIDTH_PX/2, (WINDOW_HEIGHT_PX-(lower_text_bounding_box[3]-lower_text_bounding_box[1]+2*WINDOW_PADDING_PX))/2, text=self.state.value.default_message, font=(FONT, FONT_SIZE, FONT_WEIGHT), fill='black', anchor='center')

  def check_if_running(self):
    if not self.running:
      self.root.destroy()
    else:
      self.root.after(100, self.check_if_running)

  def main_loop(self):
    try:
      self.root.after(100, self.check_if_running)
      self.root.mainloop()
    except Exception as e:
      print('Error in tkinter_gui main loop:', str(e))
      self.set_system_status(SystemStatus.ERROR)

  def enqueue_state(self, state: State, display_text: str = ''):
    self.state_queue.append(StateWithMessage(state=state, message=state.value.default_message + display_text))

  def process_state_queue(self):
    try:
      while self.running:
        if len(self.state_queue) == 0:
          continue
        state, display_text = None, None
        # when in 'VALIDATING' state, ensure the result of the validation is the next state selected
        if self.state == State.VALIDATING:
          found_validation_result = False
          for state_with_message in list(self.state_queue):
            # only want to transition to a success or failure failure state related to the validation, not a failure state caused by someone walking through with no tag
            if (state_with_message.state == State.SUCCESS or state_with_message.state == State.FAILURE) and state_with_message.message != 'Valid payment fob not found.':
              found_validation_result = True
              state, display_text = state_with_message
              self.state_queue.remove(state_with_message)
              break
          if not found_validation_result:
            if (datetime.now() - self.validation_start_time).total_seconds() < VALIDATION_UI_TIMEOUT_S: # not 100% sure this timeout working
              continue
            # time in validation exceeds timeout, assume no there will be no response from stm board
            self.validation_start_time = None
            state = State.FAILURE
            display_text = state.value.default_message
        else:
          state, display_text = self.state_queue.popleft()
        self.state = state
        background = state.value.color
        self.canvas.config(bg=background)
        self.canvas.itemconfig(self.main_text, text=display_text)
        # add waiting state to the queue for after failure or success state, to be shown after TIME_IN_SUCCESS_OR_FAILURE_STATE_S seconds
        if state == State.SUCCESS or state == State.FAILURE:
          #os.system(f"aplay Sounds/{'success' if state == State.SUCCESS else 'failure'}.wav")
          self.state_queue.append(StateWithMessage(state=State.WAITING, message=State.WAITING.value.default_message))
          time.sleep(TIME_IN_SUCCESS_OR_FAILURE_STATE_S)
        elif state == State.VALIDATING:
          self.validation_start_time = datetime.now()
    except Exception as e:
      print('Error in tkinter_gui state queue loop:', str(e))
      self.set_system_status(SystemStatus.ERROR)
  
  def increment_person_counter(self):
    self.num_people_on_bus += 1
    self.canvas.itemconfig(self.person_counter_text, text=self.get_person_counter_string())

  def decrement_person_counter(self):
    if self.num_people_on_bus > 0:
      self.num_people_on_bus -= 1
    self.canvas.itemconfig(self.person_counter_text, text=self.get_person_counter_string())

  def get_person_counter_string(self):
    return f'{PERSON_COUNTER_TEXT_TEMPLATE}{self.num_people_on_bus}'
  
  def set_system_status(self, system_status):
    self.system_status = system_status
    self.canvas.itemconfig(self.status_text, text=system_status.value.default_message, fill=system_status.value.color)
    self.canvas.coords(self.status_template_text, self.canvas.bbox(self.status_text)[0], WINDOW_HEIGHT_PX-WINDOW_PADDING_PX)

  def initiate_reset(self):
    self.on_reset()
    self.fob_processing.initiate_reset()
    self.canvas.itemconfig(self.person_counter_text, text=self.get_person_counter_string())
    self.canvas.itemconfig(self.status_text, text=self.system_status.value.default_message, fill=self.system_status.value.color)
    self.canvas.coords(self.status_template_text, self.canvas.bbox(self.status_text)[0], WINDOW_HEIGHT_PX-WINDOW_PADDING_PX)
    self.canvas.itemconfig(self.main_text, text=self.state.value.default_message)
    self.canvas.config(bg=self.state.value.color)

  def on_reset(self):
    self.num_people_on_bus = 0
    self.system_status = SystemStatus.RUNNING
    self.state = State.WAITING
    self.validation_start_time = None
    self.state_queue = deque()
