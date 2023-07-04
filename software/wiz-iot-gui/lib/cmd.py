import time
import socket
from enum import Enum
from PySide6.QtCore import QThread, QObject, Signal


CMD_STX0 = 0x02
CMD_STX1 = 0xFD

PKT_TYPE_CMD   = 0x00
PKT_TYPE_RESP  = 0x01
PKT_TYPE_EVENT = 0x02
PKT_TYPE_LOG   = 0x03
PKT_TYPE_PING  = 0x04



def millis():
  return round(time.time() * 1000)


class CmdThread(QThread):
  rxd_sig = Signal(bytes, str, str)

  def __init__(self, sock):
    super().__init__()
    self.working = True
    self.request_exit = False    
    self.sock = sock

  def __del__(self):
    pass
    
  def run(self):
    while self.working:
      try:
        data, addr = self.sock.recvfrom(1024)
        print(addr)
        self.rxd_sig.emit(data, str(addr[0]), str(addr[1]))
      except Exception as e:
        if self.request_exit == True:
          self.working = False

  def setRxdSignal(self, receive_func):
    self.rxd_sig.connect(receive_func)

  def stop(self):
    self.request_exit = True
    self.quit()
    while True:
      self.sleep(0.1)
      if self.working == False:
        break
    

class Cmd:
  def __init__(self):
    self.is_init = False
    self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self.sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)

    self.rxd_thread = CmdThread(self.sock)    
    self.rxd_thread.start()    


  def __del__(self):
    self.sock.close()
    self.rxd_thread.stop()
    print("sock.close()")

  def init(self):
    return
  
  def open(self):
    return

  def send(self, type, cmd, err_code, data, length):
    index = 0
    buffer = bytearray(10 + length)
    buffer[0] = CMD_STX0
    buffer[1] = CMD_STX1
    buffer[2] = type & 0xFF
    buffer[3] = (cmd >> 0) & 0xFF
    buffer[4] = (cmd >> 8) & 0xFF
    buffer[5] = (err_code >> 0) & 0xFF
    buffer[6] = (err_code >> 8) & 0xFF
    buffer[7] = (length >> 0) & 0xFF
    buffer[8] = (length >> 8) & 0xFF

    index = 9
    for i in range(length):
      buffer[index+i] = data[i]

    index += length

    check_sum = 0
    for i in range(index):
      check_sum += buffer[i]

    check_sum = (~check_sum) + 1
    buffer[index] = check_sum & 0xFF
    index += 1

    tx_len = self.sock.sendto(buffer, ("255.255.255.255", 5000))

  def setRxdSignal(self, rxd_func):
    self.rxd_thread.setRxdSignal(rxd_func)

  def print(self):
    pre_time = millis()
    time.sleep(0.1)
    exe_time = millis()-pre_time
    print("cmd test " + str(exe_time))