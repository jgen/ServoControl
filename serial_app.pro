#-------------------------------------------------
#
# Project created by QtCreator 2011-01-10T12:51:05
#
#-------------------------------------------------

QT       += core gui network

TARGET = serial_app
TEMPLATE = app

unix:include(src/qserialdevice/unix/ttylocker.pri)
include(src/qserialdevice/qserialdeviceenumerator/qserialdeviceenumerator.pri)
include(src/qserialdevice/qserialdevice/qserialdevice.pri)

SOURCES += src/main.cpp\
    src/serial_widget.cpp \
    src/infowidget.cpp \
    src/tracewidget.cpp \
    src/mainwindow.cpp \
    src/logviewer.cpp \
    src/logviewerdialog.cpp \
    src/network_widget.cpp \
    src/servoboard_main.cpp \
    src/serialwrapper.cpp \
    src/connectioncontroller.cpp \
    src/position.cpp \
    src/sequence.cpp \
    src/advancedlineoptionsdialog.cpp \
    src/servoboardcontroller.cpp
HEADERS += src/serial_widget.h \
    src/infowidget.h \
    src/tracewidget.h \
    src/mainwindow.h \
    src/logviewer.h \
    src/logviewerdialog.h \
    src/network_widget.h \
    src/servoboard_main.h \
    src/serialwrapper.h \
    src/connectioncontroller.h \
    src/position.h \
    src/sequence.h \
    src/advancedlineoptionsdialog.h \
    src/servoboardcontroller.h
FORMS += src/serial_widget.ui \
    src/infowidget.ui \
    src/tracewidget.ui \
    src/mainwindow.ui \
    src/network_widget.ui \
    src/servoboard_main.ui \
    src/advancedlineoptionsdialog.ui

win32 {
    LIBS += -lsetupapi -luuid -ladvapi32
}
unix:!macx {
    LIBS += -ludev
}

OTHER_FILES += \
    TODO.txt

RESOURCES += \
    src/SerialPortIcons.qrc
