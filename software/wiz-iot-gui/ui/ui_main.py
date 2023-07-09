# -*- coding: utf-8 -*-

################################################################################
## Form generated from reading UI file 'ui_main.ui'
##
## Created by: Qt User Interface Compiler version 6.5.0
##
## WARNING! All changes made in this file will be lost when recompiling UI file!
################################################################################

from PySide6.QtCore import (QCoreApplication, QDate, QDateTime, QLocale,
    QMetaObject, QObject, QPoint, QRect,
    QSize, QTime, QUrl, Qt)
from PySide6.QtGui import (QAction, QBrush, QColor, QConicalGradient,
    QCursor, QFont, QFontDatabase, QGradient,
    QIcon, QImage, QKeySequence, QLinearGradient,
    QPainter, QPalette, QPixmap, QRadialGradient,
    QTransform)
from PySide6.QtWidgets import (QApplication, QComboBox, QFrame, QHBoxLayout,
    QMainWindow, QMenu, QMenuBar, QPlainTextEdit,
    QProgressBar, QPushButton, QRadioButton, QSizePolicy,
    QSpacerItem, QStatusBar, QTabWidget, QVBoxLayout,
    QWidget)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        if not MainWindow.objectName():
            MainWindow.setObjectName(u"MainWindow")
        MainWindow.resize(900, 562)
        font = QFont()
        font.setPointSize(14)
        MainWindow.setFont(font)
        self.actionForce_Exit = QAction(MainWindow)
        self.actionForce_Exit.setObjectName(u"actionForce_Exit")
        self.actionClear = QAction(MainWindow)
        self.actionClear.setObjectName(u"actionClear")
        self.centralwidget = QWidget(MainWindow)
        self.centralwidget.setObjectName(u"centralwidget")
        self.horizontalLayout = QHBoxLayout(self.centralwidget)
        self.horizontalLayout.setObjectName(u"horizontalLayout")
        self.frame = QFrame(self.centralwidget)
        self.frame.setObjectName(u"frame")
        self.frame.setMinimumSize(QSize(200, 0))
        self.frame.setFrameShape(QFrame.StyledPanel)
        self.frame.setFrameShadow(QFrame.Raised)
        self.verticalLayout_2 = QVBoxLayout(self.frame)
#ifndef Q_OS_MAC
        self.verticalLayout_2.setSpacing(-1)
#endif
        self.verticalLayout_2.setObjectName(u"verticalLayout_2")
        self.btn_scan = QPushButton(self.frame)
        self.btn_scan.setObjectName(u"btn_scan")

        self.verticalLayout_2.addWidget(self.btn_scan)

        self.combo_device = QComboBox(self.frame)
        self.combo_device.setObjectName(u"combo_device")

        self.verticalLayout_2.addWidget(self.combo_device)

        self.text_info = QPlainTextEdit(self.frame)
        self.text_info.setObjectName(u"text_info")
        sizePolicy = QSizePolicy(QSizePolicy.Ignored, QSizePolicy.Minimum)
        sizePolicy.setHorizontalStretch(0)
        sizePolicy.setVerticalStretch(0)
        sizePolicy.setHeightForWidth(self.text_info.sizePolicy().hasHeightForWidth())
        self.text_info.setSizePolicy(sizePolicy)
        self.text_info.setMinimumSize(QSize(174, 0))
        self.text_info.setCursorWidth(0)

        self.verticalLayout_2.addWidget(self.text_info)

        self.verticalSpacer = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.verticalLayout_2.addItem(self.verticalSpacer)


        self.horizontalLayout.addWidget(self.frame)

        self.tabWidget = QTabWidget(self.centralwidget)
        self.tabWidget.setObjectName(u"tabWidget")
        self.tab_audio = QWidget()
        self.tab_audio.setObjectName(u"tab_audio")
        self.verticalLayout_3 = QVBoxLayout(self.tab_audio)
        self.verticalLayout_3.setObjectName(u"verticalLayout_3")
        self.frame_2 = QFrame(self.tab_audio)
        self.frame_2.setObjectName(u"frame_2")
        self.frame_2.setMinimumSize(QSize(0, 300))
        self.frame_2.setFrameShape(QFrame.StyledPanel)
        self.frame_2.setFrameShadow(QFrame.Raised)
        self.verticalLayout_4 = QVBoxLayout(self.frame_2)
        self.verticalLayout_4.setObjectName(u"verticalLayout_4")
        self.horizontalLayout_2 = QHBoxLayout()
        self.horizontalLayout_2.setObjectName(u"horizontalLayout_2")
        self.combo_file = QComboBox(self.frame_2)
        self.combo_file.setObjectName(u"combo_file")

        self.horizontalLayout_2.addWidget(self.combo_file)

        self.btn_open = QPushButton(self.frame_2)
        self.btn_open.setObjectName(u"btn_open")
        sizePolicy1 = QSizePolicy(QSizePolicy.Fixed, QSizePolicy.Fixed)
        sizePolicy1.setHorizontalStretch(0)
        sizePolicy1.setVerticalStretch(0)
        sizePolicy1.setHeightForWidth(self.btn_open.sizePolicy().hasHeightForWidth())
        self.btn_open.setSizePolicy(sizePolicy1)
        self.btn_open.setMinimumSize(QSize(0, 0))

        self.horizontalLayout_2.addWidget(self.btn_open)


        self.verticalLayout_4.addLayout(self.horizontalLayout_2)

        self.horizontalLayout_4 = QHBoxLayout()
        self.horizontalLayout_4.setObjectName(u"horizontalLayout_4")
        self.horizontalLayout_4.setContentsMargins(0, -1, -1, -1)
        self.btn_play = QPushButton(self.frame_2)
        self.btn_play.setObjectName(u"btn_play")
        sizePolicy2 = QSizePolicy(QSizePolicy.Minimum, QSizePolicy.Fixed)
        sizePolicy2.setHorizontalStretch(0)
        sizePolicy2.setVerticalStretch(0)
        sizePolicy2.setHeightForWidth(self.btn_play.sizePolicy().hasHeightForWidth())
        self.btn_play.setSizePolicy(sizePolicy2)

        self.horizontalLayout_4.addWidget(self.btn_play)

        self.btn_stop = QPushButton(self.frame_2)
        self.btn_stop.setObjectName(u"btn_stop")

        self.horizontalLayout_4.addWidget(self.btn_stop)

        self.radio_i2s = QRadioButton(self.frame_2)
        self.radio_i2s.setObjectName(u"radio_i2s")

        self.horizontalLayout_4.addWidget(self.radio_i2s)

        self.radio_sai = QRadioButton(self.frame_2)
        self.radio_sai.setObjectName(u"radio_sai")

        self.horizontalLayout_4.addWidget(self.radio_sai)

        self.horizontalSpacer = QSpacerItem(40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)

        self.horizontalLayout_4.addItem(self.horizontalSpacer)


        self.verticalLayout_4.addLayout(self.horizontalLayout_4)

        self.prog_bar_play = QProgressBar(self.frame_2)
        self.prog_bar_play.setObjectName(u"prog_bar_play")
        sizePolicy3 = QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Fixed)
        sizePolicy3.setHorizontalStretch(0)
        sizePolicy3.setVerticalStretch(0)
        sizePolicy3.setHeightForWidth(self.prog_bar_play.sizePolicy().hasHeightForWidth())
        self.prog_bar_play.setSizePolicy(sizePolicy3)
        self.prog_bar_play.setMinimumSize(QSize(0, 0))
        self.prog_bar_play.setValue(0)

        self.verticalLayout_4.addWidget(self.prog_bar_play)

        self.text_file_info = QPlainTextEdit(self.frame_2)
        self.text_file_info.setObjectName(u"text_file_info")
        self.text_file_info.setCursorWidth(0)

        self.verticalLayout_4.addWidget(self.text_file_info)

        self.verticalSpacer_2 = QSpacerItem(20, 40, QSizePolicy.Minimum, QSizePolicy.Expanding)

        self.verticalLayout_4.addItem(self.verticalSpacer_2)


        self.verticalLayout_3.addWidget(self.frame_2)

        self.log_text = QPlainTextEdit(self.tab_audio)
        self.log_text.setObjectName(u"log_text")

        self.verticalLayout_3.addWidget(self.log_text)

        self.tabWidget.addTab(self.tab_audio, "")
        self.tab_info = QWidget()
        self.tab_info.setObjectName(u"tab_info")
        self.verticalLayout = QVBoxLayout(self.tab_info)
        self.verticalLayout.setObjectName(u"verticalLayout")
        self.tabWidget.addTab(self.tab_info, "")

        self.horizontalLayout.addWidget(self.tabWidget)

        MainWindow.setCentralWidget(self.centralwidget)
        self.menubar = QMenuBar(MainWindow)
        self.menubar.setObjectName(u"menubar")
        self.menubar.setGeometry(QRect(0, 0, 900, 24))
        self.menuFile = QMenu(self.menubar)
        self.menuFile.setObjectName(u"menuFile")
        self.menuLog = QMenu(self.menubar)
        self.menuLog.setObjectName(u"menuLog")
        MainWindow.setMenuBar(self.menubar)
        self.statusbar = QStatusBar(MainWindow)
        self.statusbar.setObjectName(u"statusbar")
        MainWindow.setStatusBar(self.statusbar)

        self.menubar.addAction(self.menuFile.menuAction())
        self.menubar.addAction(self.menuLog.menuAction())
        self.menuFile.addAction(self.actionForce_Exit)
        self.menuLog.addAction(self.actionClear)

        self.retranslateUi(MainWindow)

        self.tabWidget.setCurrentIndex(0)


        QMetaObject.connectSlotsByName(MainWindow)
    # setupUi

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QCoreApplication.translate("MainWindow", u"WIZ-IOT-GUI", None))
        self.actionForce_Exit.setText(QCoreApplication.translate("MainWindow", u"Force Exit", None))
        self.actionClear.setText(QCoreApplication.translate("MainWindow", u"Clear", None))
        self.btn_scan.setText(QCoreApplication.translate("MainWindow", u"Scan", None))
        self.btn_open.setText(QCoreApplication.translate("MainWindow", u"Open", None))
        self.btn_play.setText(QCoreApplication.translate("MainWindow", u"Play", None))
        self.btn_stop.setText(QCoreApplication.translate("MainWindow", u"Stop", None))
        self.radio_i2s.setText(QCoreApplication.translate("MainWindow", u"I2S", None))
        self.radio_sai.setText(QCoreApplication.translate("MainWindow", u"SAI", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_audio), QCoreApplication.translate("MainWindow", u"Audio", None))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tab_info), QCoreApplication.translate("MainWindow", u"Info", None))
        self.menuFile.setTitle(QCoreApplication.translate("MainWindow", u"File", None))
        self.menuLog.setTitle(QCoreApplication.translate("MainWindow", u"Log", None))
    # retranslateUi

