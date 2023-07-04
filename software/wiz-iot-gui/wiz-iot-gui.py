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





class MainWindow(QMainWindow):
    
  def __init__(self):
    super(MainWindow, self).__init__()
    self.ui = Ui_MainWindow()
    self.ui.setupUi(self)

    self.log = LogWidget(self.ui.log_text)
    self.log.printLog('WIZ-IOT-GUI 23-06-16\n')

    self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)


    self.cmd = Cmd()
    self.cmd.setRxdSignal(self.rxdSignal)
    
    self.setClickedEvent(self.ui.btn_scan, self.btnScan)  

  def setClickedEvent(self, event_dst, event_func):
    event_dst.clicked.connect(lambda: self.btnClicked(event_dst, event_func))   

  def btnClicked(self, button, event_func):
    print(button.text())
    event_func()

  def btnScan(self):
    self.ui.combo_device.clear()
    self.cmd.send(PKT_TYPE_CMD, 0x0001, 0, bytes(10), 10)

  def rxdSignal(self, data, str_ip, str_port):
    print("rxd")
    print(str_ip)
    print(str_port)
    print(data)
    self.ui.combo_device.addItem(str_ip)
    


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