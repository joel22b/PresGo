from collections import deque
from enum import Enum
import math
import threading
import time
import tkinter

WINDOW_TITLE = 'PresGo GUI'
WINDOW_WIDTH = 800 # in px
WINDOW_HEIGHT = 800 # in px
TIME_IN_SUCCESS_OR_FAILURE_STATE = 1 # in s
FONT = 'Arial'

class TkinterGUI:
  def __init__(self, close_event):
    self.close_event = close_event
    self.state_queue = deque()
    self.state_thread = threading.Thread(target=self.process_state_queue, daemon=True)
    self.state_thread.start()
    self.root = tkinter.Tk()
    self.root.title(WINDOW_TITLE)
    self.canvas = tkinter.Canvas(self.root, width=WINDOW_WIDTH, height=WINDOW_HEIGHT, bg=State.WAITING.value[0])
    self.canvas.pack(fill='both', expand=True)
    self.gui_main_text = self.canvas.create_text(math.ceil(WINDOW_WIDTH/2), math.ceil(WINDOW_HEIGHT/2), text=State.WAITING.value[1], font=(FONT, math.ceil(WINDOW_WIDTH/40)))
    self.gui_status_text = self.canvas.create_text(WINDOW_WIDTH-math.ceil(WINDOW_WIDTH/40), math.ceil(WINDOW_HEIGHT/40), text='System Status: Running', font=(FONT, math.ceil(WINDOW_WIDTH/60)), fill=State.SUCCESS.value[0])
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
        state, display_text = self.state_queue.popleft()
        background = state.value[0]
        display_text = display_text if display_text is not None else state.value[1]
        self.canvas.config(bg=background)
        self.canvas.itemconfig(self.gui_main_text, text=display_text)
        if state == State.SUCCESS or state == State.FAILURE:
          time.sleep(TIME_IN_SUCCESS_OR_FAILURE_STATE)

  def set_state(self, state, display_text=None):
    self.state_queue.append((state, display_text))
    if state == State.SUCCESS or state == State.FAILURE:
      self.state_queue.append((State.WAITING, State.WAITING.value[1]))


class State(Enum):
  WAITING = ('grey', 'Waiting for passengers...')
  VALIDATING = ('yellow', 'Attempting to process payment')
  SUCCESS = ('green', 'Payment completed successfully')
  FAILURE = ('red', 'Payment was not successful')