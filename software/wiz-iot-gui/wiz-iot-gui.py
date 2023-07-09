import sys
import time
import os
import socket

from os import path
from PySide6.QtWidgets import QApplication, QMainWindow
from PySide6.QtCore import QFile
from PySide6.QtGui import *
from ui.ui_main import *
from lib.log import LogWidget
from lib.cmd import *
from struct import *




class MainWindow(QMainWindow):
    
  def __init__(self):
    super(MainWindow, self).__init__()
    self.ui = Ui_MainWindow()
    self.ui.setupUi(self)

    self.log = LogWidget(self.ui.log_text)
    self.log.printLog('WIZ-IOT-GUI 23-06-16\n')

    self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


    self.cmd = Cmd()
    # self.cmd.setRxdSignal(self.rxdSignal)
    
    self.setClickedEvent(self.ui.btn_scan, self.btnScan)  

  def setClickedEvent(self, event_dst, event_func):
    event_dst.clicked.connect(lambda: self.btnClicked(event_dst, event_func))   

  def btnClicked(self, button, event_func):
    print(button.text())
    event_func()

  def btnScan(self):
    self.ui.combo_device.clear()
    self.ui.text_info.clear()
    ret, packet = self.cmd.sendCmdRxResp(BOOT_CMD_VERSION, None, 0, 1000)
    if ret == True:
      self.ui.combo_device.addItem(packet.str_ip)
      str_fmt = "I32s32sI"
      fmt_size = calcsize(str_fmt)
      data = unpack(str_fmt, packet.data[:fmt_size])
      self.ui.text_info.appendPlainText(data[2].decode("utf-8"))
      self.ui.text_info.appendPlainText("    " + data[1].decode("utf-8"))
      self.ui.text_info.appendPlainText("    ")

      data = unpack(str_fmt, packet.data[fmt_size:fmt_size*2])
      self.ui.text_info.appendPlainText(data[2].decode("utf-8"))
      self.ui.text_info.appendPlainText("    " + data[1].decode("utf-8"))


  def rxdSignal(self, packet: CmdPacket):
    print(packet.str_ip)
    print(packet.str_port)
    self.ui.combo_device.addItem(packet.str_ip)
    


  def closeEvent(self, QCloseEvent):
    self.sock.close()
    QCloseEvent.accept()





def main():
  app = QApplication(sys.argv)


  fontpath = os.path.abspath("data/font/neodgm_code.ttf")
  QFontDatabase.addApplicationFont(fontpath)
  app.setFont(QFont("NeoDunggeunmo Code", 14))


  window = MainWindow()
  center = QScreen.availableGeometry(QApplication.primaryScreen()).center()
  geo = window.frameGeometry()
  geo.moveCenter(center)
  window.move(geo.topLeft())
  window.show()

  sys.exit(app.exec())


if __name__ == "__main__":  
  main()