QT       += core gui serialport
QT       += charts
QT       += opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = single_molecule32
TEMPLATE = app

win32:LIBS += -lOpengl32 \
              -lglu32 \


CONFIG += c++11
#CONFIG += console

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += QCUSTOMPLOT_USE_OPENGL

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ./logger/

RC_ICONS = resources/images/BGI_LOGO.ico


SOURCES += \
    function/DBSCAN.cpp \
    function/clustering.cpp \
    function/gating_detection.cpp \
    helper/appconfig.cpp \
    helper/fileoperationhelper.cpp \
    helper/messagedialog.cpp \
    logger/spdlog/EcoliLog.cpp \
    main.cpp \
    models/DataBase.cpp \
    models/DataCapCaculate.cpp \
    models/DataCapCollection.cpp \
    models/DataCollection.cpp \
    models/DataFunction.cpp \
    models/DataMeansCal.cpp \
    models/DataMonitor.cpp \
    models/DataOutputFormat.cpp \
    models/DataPackageMode.cpp \
    models/DataRecord.cpp \
    models/DataStackHandle.cpp \
    models/DatacCommunicaor.cpp \
    models/SerialPortControl.cpp \
    models/SerialPortOut.cpp \
    models/costomplot/qcustomplot.cpp \
    view/aboutbox.cpp \
    view/calibrationdialog.cpp \
    view/capitem.cpp \
    view/caplistitem.cpp \
    view/cappanelview.cpp \
    view/channelitem.cpp \
    view/confirmmessagedialog.cpp \
    view/controlpanel.cpp \
    view/currentdispitem.cpp \
    view/currentdisplayview.cpp \
    view/currentsingelsettingpage.cpp \
    view/degatparamsdialog.cpp \
    view/filerecordsettingdialog.cpp \
    view/fileselectdialog.cpp \
    view/inputmessagedialog.cpp \
    view/logpage.cpp \
    view/mainwindow.cpp \
    view/plotreviewdialog.cpp \
    view/plottranstotextdialog.cpp \
    view/qplotwidget.cpp \
    view/savefilepanel.cpp \
    view/serialsettingsdialog.cpp \
    view/singlechcomb.cpp

HEADERS += \
    function/DBSCAN.h \
    function/clustering.h \
    helper/EnumString.h \
    helper/ExecuteListener.h \
    helper/Point.h \
    helper/Singleton.h \
    helper/appconfig.h \
    helper/commonhelper.h \
    helper/fileoperationhelper.h \
    helper/messagedialog.h \
    helper/messagedialogdefine.h \
    helper/stylesheethelper.h \
    helper/version.h \
    logger/Log.h \
    logger/LogEventParser.h \
    logger/ilogeventupdate.h \
    logger/spdlog/EcoliLog.h \
    logger/spdlog/async.h \
    logger/spdlog/async_logger.h \
    logger/spdlog/common.h \
    logger/spdlog/details/async_logger_impl.h \
    logger/spdlog/details/circular_q.h \
    logger/spdlog/details/console_globals.h \
    logger/spdlog/details/file_helper.h \
    logger/spdlog/details/fmt_helper.h \
    logger/spdlog/details/log_msg.h \
    logger/spdlog/details/logger_impl.h \
    logger/spdlog/details/mpmc_blocking_q.h \
    logger/spdlog/details/null_mutex.h \
    logger/spdlog/details/os.h \
    logger/spdlog/details/pattern_formatter.h \
    logger/spdlog/details/periodic_worker.h \
    logger/spdlog/details/registry.h \
    logger/spdlog/details/thread_pool.h \
    logger/spdlog/fmt/bundled/colors.h \
    logger/spdlog/fmt/bundled/core.h \
    logger/spdlog/fmt/bundled/format-inl.h \
    logger/spdlog/fmt/bundled/format.h \
    logger/spdlog/fmt/bundled/locale.h \
    logger/spdlog/fmt/bundled/ostream.h \
    logger/spdlog/fmt/bundled/posix.h \
    logger/spdlog/fmt/bundled/printf.h \
    logger/spdlog/fmt/bundled/ranges.h \
    logger/spdlog/fmt/bundled/time.h \
    logger/spdlog/fmt/fmt.h \
    logger/spdlog/fmt/ostr.h \
    logger/spdlog/formatter.h \
    logger/spdlog/logger.h \
    logger/spdlog/sinks/android_sink.h \
    logger/spdlog/sinks/ansicolor_sink.h \
    logger/spdlog/sinks/base_sink.h \
    logger/spdlog/sinks/basic_file_sink.h \
    logger/spdlog/sinks/daily_file_sink.h \
    logger/spdlog/sinks/dist_sink.h \
    logger/spdlog/sinks/msvc_sink.h \
    logger/spdlog/sinks/null_sink.h \
    logger/spdlog/sinks/ostream_sink.h \
    logger/spdlog/sinks/rotating_file_sink.h \
    logger/spdlog/sinks/sink.h \
    logger/spdlog/sinks/stdout_color_sinks.h \
    logger/spdlog/sinks/stdout_sinks.h \
    logger/spdlog/sinks/syslog_sink.h \
    logger/spdlog/sinks/wincolor_sink.h \
    logger/spdlog/spdlog.h \
    logger/spdlog/tweakme.h \
    logger/spdlog/version.h \
    models/DataBase.h \
    models/DataCapCaculate.h \
    models/DataCapCollection.h \
    models/DataCollection.h \
    models/DataDefine.h \
    models/DataFunction.h \
    models/DataMeansCal.h \
    models/DataMonitor.h \
    models/DataOutputFormat.h \
    models/DataPackageMode.h \
    models/DataRecord.h \
    models/DataStackHandle.h \
    models/DatacCommunicaor.h \
    models/SerialPortControl.h \
    models/SerialPortOut.h \
    models/costomplot/qcustomplot.h \
    view/aboutbox.h \
    view/calibrationdialog.h \
    view/capitem.h \
    view/caplistitem.h \
    view/cappanelview.h \
    view/channelitem.h \
    view/confirmmessagedialog.h \
    view/controlpanel.h \
    view/currentdispitem.h \
    view/currentdisplayview.h \
    view/currentsingelsettingpage.h \
    view/degatparamsdialog.h \
    view/filerecordsettingdialog.h \
    view/fileselectdialog.h \
    view/inputmessagedialog.h \
    view/logpage.h \
    view/mainwindow.h \
    view/plotreviewdialog.h \
    view/plottranstotextdialog.h \
    view/qplotwidget.h \
    view/savefilepanel.h \
    view/serialsettingsdialog.h \
    view/singlechcomb.h

FORMS += \
    view/aboutbox.ui \
    view/calibrationdialog.ui \
    view/capitem.ui \
    view/caplistitem.ui \
    view/cappanelview.ui \
    view/channelitem.ui \
    view/confirmmessagedialog.ui \
    view/controlpanel.ui \
    view/currentdispitem.ui \
    view/currentdisplayview.ui \
    view/currentsingelsettingpage.ui \
    view/degatparamsdialog.ui \
    view/filerecordsettingdialog.ui \
    view/fileselectdialog.ui \
    view/inputmessagedialog.ui \
    view/logpage.ui \
    view/mainwindow.ui \
    view/plotreviewdialog.ui \
    view/plottranstotextdialog.ui \
    view/qplotwidget.ui \
    view/savefilepanel.ui \
    view/serialsettingsdialog.ui \
    view/singlechcomb.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES +=

RESOURCES += \
    resources/rc.qrc
