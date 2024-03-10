import fob_processing
import signal
import time
from tkinter_gui import SystemStatus

fob_processing_obj = None

def signal_handler(sig, frame):
  fob_processing_obj.gui.running = False
  fob_processing_obj.pt_serial.running = False

def main():
  try:
    # setup signal handler for graceful shutdown
    signal.signal(signal.SIGINT, signal_handler)
    # setup gui, fob_processing, mqtt client, and serial communication with BlueNRG board
    global fob_processing_obj
    fob_processing_obj = fob_processing.FobProcessing()
    # start GUI loop, all tkinter operations are performed on the main thread
    fob_processing_obj.gui.main_loop()
  except Exception as e:
    print('Error on main thread:', str(e))
    if fob_processing_obj.gui:
      fob_processing_obj.gui.set_system_status(SystemStatus.ERROR)

if __name__ == "__main__":
  main()
