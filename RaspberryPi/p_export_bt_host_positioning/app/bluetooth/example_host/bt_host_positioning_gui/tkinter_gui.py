from collections import deque, namedtuple
from enum import Enum
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
TIME_IN_SUCCESS_OR_FAILURE_STATE_S = 1.5

ColorWithDefaultMessage = namedtuple('ColorWithDefaultMessage', ['color', 'default_message'])
StateWithMessage = namedtuple('StateWithMessage', ['state', 'message'])

class SystemStatus(Enum):
  RUNNING = ColorWithDefaultMessage(color='green', default_message='Running')
  ERROR = ColorWithDefaultMessage(color='red', default_message='Error')

class State(Enum):
  WAITING = ColorWithDefaultMessage(color='grey', default_message='Waiting for passengers...')
  VALIDATING = ColorWithDefaultMessage(color='yellow', default_message='Attempting to process payment...')
  SUCCESS = ColorWithDefaultMessage(color='green', default_message='Payment completed successfully.')
  FAILURE = ColorWithDefaultMessage(color='red', default_message='Payment was not successful.')

class TkinterGUI:
  def __init__(self):
    # setup graceful shutdown
    self.running = True 
    # initialize person counter, system status, state and state queue handling thread
    self.lock = Lock()
    self.reset(True)
    self.state_thread = threading.Thread(target=self.process_state_queue, daemon=True)
    self.state_thread.start()
    # initialize gui
    self.root = tkinter.Tk()
    self.root.title(WINDOW_TITLE)
    self.canvas = tkinter.Canvas(self.root, width=WINDOW_WIDTH_PX, height=WINDOW_HEIGHT_PX, bg=self.state.value.color)
    self.canvas.pack(fill='both', expand=True)
    self.gui_person_counter_text = self.canvas.create_text(WINDOW_PADDING_PX, WINDOW_HEIGHT_PX-WINDOW_PADDING_PX, text=self.get_person_counter_string(), font=(FONT, FONT_SIZE, FONT_WEIGHT), fill='black', anchor='sw')
    self.gui_status_text = self.canvas.create_text(WINDOW_WIDTH_PX-WINDOW_PADDING_PX, WINDOW_HEIGHT_PX-WINDOW_PADDING_PX, text=self.system_status.value.default_message, font=(FONT, FONT_SIZE, FONT_WEIGHT), fill=self.system_status.value.color, anchor='se')
    self.canvas.create_text(self.canvas.bbox(self.gui_status_text)[0], WINDOW_HEIGHT_PX-WINDOW_PADDING_PX, text=STATUS_TEXT_TEMPLATE, font=(FONT, FONT_SIZE, FONT_WEIGHT), fill='black', anchor='se')
    lower_text_bounding_box = self.canvas.bbox(self.gui_person_counter_text)
    gui_bottom_rectangle = self.canvas.create_rectangle(lower_text_bounding_box[0]-WINDOW_PADDING_PX, lower_text_bounding_box[1]-WINDOW_PADDING_PX, WINDOW_WIDTH_PX, lower_text_bounding_box[3]+WINDOW_PADDING_PX, fill='#3B3B3B')                                 
    self.canvas.tag_lower(gui_bottom_rectangle, self.gui_person_counter_text)
    self.gui_main_text = self.canvas.create_text(WINDOW_WIDTH_PX/2, (WINDOW_HEIGHT_PX-(lower_text_bounding_box[3]-lower_text_bounding_box[1]+2*WINDOW_PADDING_PX))/2, text=self.state.value.default_message, font=(FONT, FONT_SIZE, FONT_WEIGHT), fill='black', anchor='center')

  def reset(self, init = False):
    with self.lock:
      self.num_people_on_bus = 0
      self.system_status = SystemStatus.RUNNING
      self.state = State.WAITING
      self.state_queue = deque()
      if not init:
        self.canvas.itemconfig(self.gui_person_counter_text, text=self.get_person_counter_string())
        self.canvas.itemconfig(self.gui_status_text, text=self.system_status.value.default_message)
        self.canvas.itemconfig(self.gui_main_text, text=self.state.value.default_message)
        self.canvas.config(bg=self.state.value.color)

  def check_if_running(self):
    if not self.running:
      self.root.destroy()
    else:
      self.root.after(100, self.check_if_running)

  def get_person_counter_string(self):
    return f'{PERSON_COUNTER_TEXT_TEMPLATE}{self.num_people_on_bus}'

  def increment_person_counter(self):
    with self.lock:
      self.num_people_on_bus += 1
      self.canvas.itemconfig(self.gui_person_counter_text, text=self.get_person_counter_string())

  def decrement_person_counter(self):
    with self.lock:
      if self.num_people_on_bus > 0:
        self.num_people_on_bus -= 1
      self.canvas.itemconfig(self.gui_person_counter_text, text=self.get_person_counter_string())

  def set_system_status(self, system_status):
    with self.lock:
      self.system_status = system_status
      self.canvas.itemconfig(self.gui_status_text, text=system_status.value.default_message)

  def enqueue_state(self, state, display_text=None):
    with self.lock:
      display_text = display_text if display_text is not None else state.value.default_message
      self.state_queue.append(StateWithMessage(state=state, message=display_text))
      # add waiting state to the queue for after failure or success state, to be shown after TIME_IN_SUCCESS_OR_FAILURE_STATE_S seconds
      if state == State.SUCCESS or state == State.FAILURE:
        self.state_queue.append(StateWithMessage(state=State.WAITING, message=State.WAITING.value.default_message))

  def process_state_queue(self):
    while self.running:
      with self.lock:
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

  def start_main_loop(self):
    self.main_thread = threading.Thread(target=self.main_loop(), daemon=True)
    self.main_thread.start()

  def main_loop(self):
    self.root.after(100, self.check_if_running)
    self.root.mainloop()