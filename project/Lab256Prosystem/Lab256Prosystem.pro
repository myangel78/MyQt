QT       += core gui serialport printsupport network
# QT       += charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
QMAKE_CXXFLAGS += -finput-charset='gbk'

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QCUSTOMPLOT_USE_OPENGL
DEFINES -= UNICODE
DEFINES -= _UNICODE

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ./include/logger/
INCLUDEPATH += ./include/

RC_ICONS = res/img/BGI_LOGO.ico


SOURCES += \
    src/AboutDialog.cpp \
    src/CalibrationDialog.cpp \
    src/CapListItem.cpp \
    src/CapUnitItem.cpp \
    src/CapacitanceView.cpp \
    src/ChannelStatusView.cpp \
    src/ConfigServer.cpp \
    src/ControlListItem.cpp \
    src/ControlView.cpp \
    src/CustomPlotView.cpp \
    src/DegatingDialog.cpp \
    src/FeedbackDialog.cpp \
    src/FlowModulePane.cpp \
    src/HistoryDataView.cpp \
    src/HttpClient.cpp \
    src/LogPane.cpp \
    src/MainWindow.cpp \
    src/MyQMessageBox.cpp \
    src/PlotReviewDialog.cpp \
    src/PoreSelectDialog.cpp \
    src/RecordSettingDialog.cpp \
    src/RedisAccess.cpp \
    src/SampleRateDialog.cpp \
    src/SavePane.cpp \
    src/SerialPort.cpp \
    src/SerialSettingDialog.cpp \
    src/TemperatureMonitor.cpp \
    src/TranslateDialog.cpp \
    src/UpdateDialog.cpp \
    src/UsbPortBulk.cpp \
    src/UsbProtoMsg.cpp \
    src/VerUpdateDialog.cpp \
    src/ZeroAdjustDialog.cpp \
    src/main.cpp \
    src/qcustomplot.cpp

HEADERS += \
    src/AboutDialog.h \
    src/CalibrationDialog.h \
    src/CapListItem.h \
    src/CapUnitItem.h \
    src/CapacitanceView.h \
    src/ChannelStatusView.h \
    src/ConfigServer.h \
    src/ControlListItem.h \
    src/ControlView.h \
    src/CustomPlotView.h \
    src/DegatingDialog.h \
    src/FeedbackDialog.h \
    src/FlowModulePane.h \
    src/HistoryDataView.h \
    src/HttpClient.h \
    src/LogPane.h \
    src/MainWindow.h \
    src/MyQMessageBox.h \
    src/PlotReviewDialog.h \
    src/PoreSelectDialog.h \
    src/RecordSettingDialog.h \
    src/RedisAccess.h \
    src/SampleRateDialog.h \
    src/SavePane.h \
    src/SerialPort.h \
    src/SerialSettingDialog.h \
    src/TemperatureMonitor.h \
    src/TranslateDialog.h \
    src/UpdateDialog.h \
    src/UsbPortBulk.h \
    src/UsbProtoMsg.h \
    src/VerUpdateDialog.h \
    src/ZeroAdjustDialog.h \
    src/mainversion.h \
    src/qcustomplot.h

win32{
INCLUDEPATH += ./include/CyApi/

SOURCES += \
    src/CyAPI.cpp \
    src/UsbCyDevice.cpp

HEADERS += \
    src/UsbCyDevice.h

LIBS += -L$$PWD/lib/ -lWS2_32
CONFIG(release, debug|release): LIBS += -L$$PWD/./ -llibcurl
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/./ -llibcurld
CONFIG(release, debug|release): LIBS += -L$$PWD/./ -llibhiredis
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/./ -llibhiredisd
DEPENDPATH += $$PWD/.
}
unix{
SOURCES += \
    src/UsbCyclone.cpp

HEADERS += \
    src/UsbCyclone.h

LIBS += -L/usr/local/libusb-1.0.24/lib/ -lusb-1.0
LIBS += -L/usr/local/curl-7.75.0/lib/ -lcurl
LIBS += -L/usr/local/lib/ -lhiredis

}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res/res.qrc
