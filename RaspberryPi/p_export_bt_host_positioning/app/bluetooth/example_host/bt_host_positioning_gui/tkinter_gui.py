from collections import deque
from enum import Enum
import math
import threading
import time
import tkinter

WINDOW_TITLE = 'PresGo GUI'
WINDOW_WIDTH_PX = 800
WINDOW_HEIGHT_PX = 800
TIME_IN_SUCCESS_OR_FAILURE_STATE_S = 1
FONT = 'Arial'

class TkinterGUI:
  def __init__(self, close_event):
    self.close_event = close_event
    self.state_queue = deque()
    self.state_thread = threading.Thread(target=self.process_state_queue, daemon=True)
    self.state_thread.start()
    self.root = tkinter.Tk()
    self.root.title(WINDOW_TITLE)
    self.state = State.WAITING
    self.canvas = tkinter.Canvas(self.root, width=WINDOW_WIDTH_PX, height=WINDOW_HEIGHT_PX, bg=State.WAITING.value[0])
    self.canvas.pack(fill='both', expand=True)
    self.gui_main_text = self.canvas.create_text(math.ceil(WINDOW_WIDTH_PX/2), math.ceil(WINDOW_HEIGHT_PX/2), text=State.WAITING.value[1], font=(FONT, math.ceil(WINDOW_WIDTH_PX/40)))
    self.gui_status_text = self.canvas.create_text(WINDOW_WIDTH_PX-math.ceil(WINDOW_WIDTH_PX/40), math.ceil(WINDOW_HEIGHT_PX/40), text='System Status: Running', font=(FONT, math.ceil(WINDOW_WIDTH_PX/60)), fill=State.SUCCESS.value[0])
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
      if self.state_queue:
        state, display_text = None, None
        if self.state == State.VALIDATING:
          found_validation_result = False
          for item in list(self.state_queue):
            if (item[0] == State.SUCCESS or item[0] == State.FAILURE) and item[1] == item[0].value[1]:
              found_validation_result = True
              state, display_text = item
              self.state_queue.remove(item)
              break
          if not found_validation_result:
            continue
        else:
          state, display_text  = self.state_queue.popleft()
        self.state = state
        background = state.value[0]
        self.canvas.config(bg=background)
        self.canvas.itemconfig(self.gui_main_text, text=display_text)
        if state == State.SUCCESS or state == State.FAILURE:
          time.sleep(TIME_IN_SUCCESS_OR_FAILURE_STATE_S)

  def set_state(self, state, display_text=None):
    display_text = display_text if display_text is not None else state.value[1]
    self.state_queue.append((state, display_text))
    if state == State.SUCCESS or state == State.FAILURE:
      self.state_queue.append((State.WAITING, State.WAITING.value[1]))


class State(Enum):
  WAITING = ('grey', 'Waiting for passengers...')
  VALIDATING = ('yellow', 'Attempting to process payment')
  SUCCESS = ('green', 'Payment completed successfully')
  FAILURE = ('red', 'Payment was not successful')