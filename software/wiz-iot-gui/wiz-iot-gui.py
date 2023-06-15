import sys
import time
import os
from os import path
from PySide6.QtWidgets import QApplication, QMainWindow
from PySide6.QtCore import QFile
from PySide6.QtGui import *
from ui.ui_main import *
from lib.log import LogWidget




class MainWindow(QMainWindow):
    
  def __init__(self):
    super(MainWindow, self).__init__()
    self.ui = Ui_MainWindow()
    self.ui.setupUi(self)

    self.log = LogWidget(self.ui.log_text)
    self.log.printLog('WIZ-IOT-GUI 23-06-16\n')


def main():
  app = QApplication(sys.argv)

  # QFontDatabase.addApplicationFont('data/font/D2Coding-Ver1.3.2-20180524.ttf')    
  #app.setFont(QFont('Neo둥근모 Code', 14))    
  # app.setFont(QFont('Monospace', 14))
  # app.setFont(QFont('D2Coding', 14))



  window = MainWindow()
  center = QScreen.availableGeometry(QApplication.primaryScreen()).center()
  geo = window.frameGeometry()
  geo.moveCenter(center)
  window.move(geo.topLeft())
  window.show()

  sys.exit(app.exec())


if __name__ == "__main__":  
  main()