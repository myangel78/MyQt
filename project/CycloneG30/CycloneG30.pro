QT       += core gui sql network serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
#CONFIG += console
#win32:CONFIG += console

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QCUSTOMPLOT_USE_OPENGL

#win32:DEFINES += _WINSOCKAPI_

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

RC_ICONS = res/img/cycloneG30.ico

INCLUDEPATH += include
INCLUDEPATH += include/logger
INCLUDEPATH += include/logger/spdlog


INCLUDEPATH += src
INCLUDEPATH += src/table
INCLUDEPATH += src/customplot
INCLUDEPATH += src/wizard




HEADERS +=\
    src/AboutDialog.h \
    src/AutoScanPanel.h \
    src/FlowOpertPanel.h \
    src/PercentBarChartView.h \
    src/CapCaculate.h \
    src/ChartViewWidgt.h \
    src/CustomSplitter.h \
    src/DataAnalysWidgt.h \
    src/ComunicateTestWdgt.h \
    src/ConfigServer.h \
    src/ConfigurationWidgt.h \
    src/CustomPlotWidgt.h \
    src/DataCalculate.h \
    src/DataHandle.h \
    src/DegatingPane.h \
    src/HttpClient.h \
    src/MembranePanel.h \
    src/ModelSharedData.h \
    src/MuxScanPanel.h \
    src/MyQMessageBox.h \
    src/PlotReviewDialog.h \
    src/PoreCtrlPanel.h \
    src/PoreStateMapWdgt.h \
    src/QNotify.h \
    src/RecordCsvFile.h \
    src/RecordSettingDialog.h \
    src/RedisAccess.h \
    src/SavePannel.h \
    src/SensorPanel.h \
    src/SerialPort.h \
    src/SerialSettingDialog.h \
    src/TemperatureMonitor.h \
    src/TranslateDialog.h \
    src/UDPSocket.h \
    src/ValidOptimized.h \
    src/VoltCtrPanel.h \
    src/customplot/qcustomplot.h \
    src/table/CustomDelegate.h \
    src/Global.h \
    src/table/InputNewRecordDlg.h \
    src/LogPane.h \
    src/MainWindow.h \
    src/table/PlotCfgDelegate.h \
    src/table/PlotCfgTabModel.h \
    src/table/PoreStateMapTabModel.h \
    src/table/RegFieldTableModel.h \
    src/RegisterMapWdgt.h \
    src/table/RegisterTableModel.h \
    src/RegisterWidget.h \
    src/SqlDataBase.h \
    src/UsbFtdDevice.h \
    src/UsbProtoMsg.h \
    src/Util.h \
    src/XmlStreamReader.h \
    src/mainversion.h \
    src/table/RegisterTableView.h\
    src/table/RegFieldTableView.h \
    src/wizard/AutoSequenceSettingDlg.h \
    src/wizard/CustomComponent.h \
    src/wizard/MembraneQCSettingDlg.h \
    src/wizard/PoreAutoFiltSettingDlg.h

SOURCES += \
    src/AboutDialog.cpp \
    src/AutoScanPanel.cpp \
    src/FlowOpertPanel.cpp \
    src/PercentBarChartView.cpp \
    src/CapCaculate.cpp \
    src/ChartViewWidgt.cpp \
    src/CustomSplitter.cpp \
    src/DataAnalysWidgt.cpp \
    src/ComunicateTestWdgt.cpp \
    src/ConfigServer.cpp \
    src/ConfigurationWidgt.cpp \
    src/CustomPlotWidgt.cpp \
    src/DataCalculate.cpp \
    src/DataHandle.cpp \
    src/DegatingPane.cpp \
    src/HttpClient.cpp \
    src/MembranePanel.cpp \
    src/ModelSharedData.cpp \
    src/MuxScanPanel.cpp \
    src/MyQMessageBox.cpp \
    src/PlotReviewDialog.cpp \
    src/PoreCtrlPanel.cpp \
    src/PoreStateMapWdgt.cpp \
    src/QNotify.cpp \
    src/RecordCsvFile.cpp \
    src/RecordSettingDialog.cpp \
    src/RedisAccess.cpp \
    src/SavePannel.cpp \
    src/SensorPanel.cpp \
    src/SerialPort.cpp \
    src/SerialSettingDialog.cpp \
    src/TemperatureMonitor.cpp \
    src/TranslateDialog.cpp \
    src/UDPSocket.cpp \
    src/ValidOptimized.cpp \
    src/VoltCtrPanel.cpp \
    src/customplot/qcustomplot.cpp \
    src/table/CustomDelegate.cpp \
    src/table/InputNewRecordDlg.cpp \
    src/LogPane.cpp \
    src/MainWindow.cpp \
    src/table/PlotCfgDelegate.cpp \
    src/table/PLotCfgTabModel.cpp \
    src/table/PoreStateMapTabModel.cpp \
    src/table/RegFieldTableModel.cpp \
    src/RegisterMapWdgt.cpp \
    src/table/RegisterTableModel.cpp \
    src/RegisterWidget.cpp \
    src/SqlDataBase.cpp \
    src/UsbFtdDevice.cpp \
    src/UsbProtoMsg.cpp \
    src/XmlStreamReader.cpp \
    src/main.cpp \
    src/table/RegisterTableView.cpp\
    src/table/RegFieldTableView.cpp \
    src/wizard/AutoSequenceSettingDlg.cpp \
    src/wizard/CustomComponent.cpp \
    src/wizard/MembraneQCSettingDlg.cpp \
    src/wizard/PoreAutoFiltSettingDlg.cpp

win32{
INCLUDEPATH += ./include/FTD3XX/
LIBS += -L$$PWD/lib/ -lFTD3XX
LIBS+= -L$$PWD/lib/ -lWS2_32
CONFIG(release, debug|release): LIBS += -L$$PWD/./ -llibcurl
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/./ -llibcurld
CONFIG(release, debug|release): LIBS += -L$$PWD/./ -llibhiredis
else:CONFIG(debug, debug|release): LIBS += -L$$PWD/./ -llibhiredisd
DEPENDPATH += $$PWD/.
}
unix{
INCLUDEPATH += ./include/ftd3xxlinux/

LIBS += -L$$PWD/lib/ -lftd3xx
LIBS += -L/usr/local/curl-7.75.0/lib/ -lcurl
LIBS += -L/usr/local/lib/ -lhiredis
}

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res/res.qrc\


