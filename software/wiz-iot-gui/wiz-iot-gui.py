import sys
import time
import os
import socket
import scipy.io as sio
import wave
from os import path
from PySide6.QtWidgets import QApplication, QMainWindow, QFileDialog
from PySide6.QtCore import QFile
from PySide6.QtGui import *
from ui.ui_main import *
from lib.log import LogWidget
from lib.cmd import *
from struct import *


class PlayThread(QThread):
  start_sig = Signal()
  finish_sig = Signal()
  update_sig = Signal(int)

  def __init__(self, parent):
    super().__init__(parent)
    self.is_run = True

  def __del__(self):
    pass

  def run(self):
    try:
      self.start_sig.emit()
      count = 0
      while self.is_run:
        time.sleep(0.1)
        if count >= 100:
          break   
        count += 1     
        self.update_sig.emit(count) 
      self.finish_sig.emit()
    except Exception as e:
      print(e)

  def stop(self):
    self.is_run = False
    self.quit()
    self.wait()


class MainWindow(QMainWindow):
    
  def __init__(self):
    super(MainWindow, self).__init__()
    self.ui = Ui_MainWindow()
    self.ui.setupUi(self)

    self.log = LogWidget(self.ui.log_text)
    self.log.printLog('WIZ-IOT-GUI 23-06-16\n')

    self.sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    self.file_list = []

    self.cmd = Cmd()
    # self.play_thread = PlayThread(self)
    self.play_thread = None
    # self.play_thread.start_sig.connect(self.onStartPlay)
    # self.play_thread.finish_sig.connect(self.onFinishPlay)
    # self.play_thread.update_sig.connect(self.onUpdatePlay)

    self.setClickedEvent(self.ui.btn_scan, self.btnScan)  
    self.setClickedEvent(self.ui.btn_open, self.btnOpen)  
    self.setClickedEvent(self.ui.btn_play, self.btnPlay)  
    self.setClickedEvent(self.ui.btn_stop, self.btnStop)  

    self.ui.combo_file.currentTextChanged.connect(self.onComboFileChanged)
    self.ui.btn_stop.setEnabled(False)

  def __del__(self):
    self.sock.close()
    self.cmd.stop()
    print("del()")

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
    
  def btnOpen(self):
    fname = QFileDialog.getOpenFileNames(self, "Open File", "", "WAV File(*.wav)")
    if len(fname[0]) > 0:
      self.updateFileList(fname)
      self.showFileInfo()

  def updateFileList(self, fname):
    self.ui.combo_file.clear()
    self.file_list.clear()
    for i in fname[0]:
      self.file_list.append(i)
      self.ui.combo_file.addItem(os.path.basename(i))

  def showFileInfo(self):

    if self.ui.combo_file.count() == 0:
      return
    
    file_name = self.file_list[self.ui.combo_file.currentIndex()]
    wav_file = wave.open(file_name)

    self.ui.text_file_info.clear()

    str_out = []
    str_out.append("{0:<20} : {1:}".format("File", os.path.basename(file_name)))
    str_out.append("{0:<20} : {1:}".format("Channels", wav_file.getnchannels()))
    str_out.append("{0:<20} : {1:}bit".format("Sample Width", wav_file.getsampwidth() * 8))
    str_out.append("{0:<20} : {1:}Hz".format("Sample Freq", wav_file.getframerate()))

    for info in str_out:
      self.ui.text_file_info.appendPlainText(info)
    wav_file.close()

  def onComboFileChanged(self, value):
    self.showFileInfo()

  def isCanPlay(self):
    if self.ui.combo_device.count() == 0:
      return False
    if self.ui.combo_file.count() == 0:
      return False
    if self.ui.btn_play.isEnabled() == False:
      return False    
    if self.play_thread is not None:
      if self.play_thread.isRunning() == True:
        return False
    return True

  def btnPlay(self):
    if self.isCanPlay() == True:
      self.play_thread = PlayThread(self)
      self.play_thread.start_sig.connect(self.onStartPlay)
      self.play_thread.finish_sig.connect(self.onFinishPlay)
      self.play_thread.update_sig.connect(self.onUpdatePlay)

      self.play_thread.start()
    else:
      self.log.printLog("Can't play")

  def btnStop(self):
    self.play_thread.stop()
    pass

  def onStartPlay(self):
    self.log.printLog("Play Start..")
    self.ui.btn_play.setEnabled(False)
    self.ui.btn_stop.setEnabled(True)

  def onFinishPlay(self):
    self.log.printLog("Play Finish..")
    self.ui.btn_play.setEnabled(True)
    self.ui.btn_stop.setEnabled(False)

  def onUpdatePlay(self, update_value):
    # self.log.printLog("update.. %d" % upate_value)
    self.ui.prog_bar_play.setValue(update_value)
    pass

  def closeEvent(self, QCloseEvent):
    if self.play_thread is not None:
      self.play_thread.stop() 
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