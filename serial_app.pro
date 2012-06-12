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

INCLUDEPATH += src/mainandlog/ \
            src/ \
            src/servoboard/ \
            src/serialtools \
            src/network\
#Main and Log

SOURCES += src/mainandlog/main.cpp \
    src/mainandlog/mainwindow.cpp \
    src/mainandlog/logviewer.cpp \
    src/mainandlog/logviewerdialog.cpp \
    src/servoboard/sequencecompletesyntaxhighlighter.cpp \

HEADERS += src/mainandlog/mainwindow.h \
    src/mainandlog/logviewer.h \
    src/mainandlog/logviewerdialog.h \
    src/servoboard/sequencecompletesyntaxhighlighter.h \


FORMS +=  src/mainandlog/mainwindow.ui \

#Servoboard

SOURCES += src/servoboard/servoboard_main.cpp \
    src/servoboard/position.cpp \
    src/servoboard/servoboardcontroller.cpp \
    src/servoboard/sequence.cpp \
    src/servoboard/sequencesyntaxhighlighter.cpp \
    src/servoboard/advancedlineoptionsdialog.cpp \
    src/servoboard/servocontrolbundle.cpp



HEADERS += src/servoboard/servoboard_main.h \
    src/servoboard/position.h \
    src/servoboard/servoboardcontroller.h \
    src/servoboard/sequence.h \
    src/servoboard/sequencesyntaxhighlighter.h \
    src/servoboard/advancedlineoptionsdialog.h \
    src/servoboard/servocontrolbundle.h


FORMS +=  src/servoboard/servoboard_main.ui \
    src/servoboard/advancedlineoptionsdialog.ui \
    src/servoboard/servocontrolbundle.ui
#Serial Connecting and Tool

SOURCES += src/serialtools/tracewidget.cpp \
    src/serialtools/serial_widget.cpp \
    src/serialtools/infowidget.cpp \
    src/serialtools/connectioncontroller.cpp \


HEADERS += src/serialtools/tracewidget.h \
    src/serialtools/serial_widget.h \
    src/serialtools/infowidget.h \
    src/serialtools/connectioncontroller.h \


FORMS +=  src/serialtools/tracewidget.ui \
    src/serialtools/serial_widget.ui \
    src/serialtools/infowidget.ui \


#Networking
SOURCES += \
    src/network/network_widget.cpp \

HEADERS += \
    src/network/network_widget.h \

FORMS += \
    src/network/network_widget.ui \

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
