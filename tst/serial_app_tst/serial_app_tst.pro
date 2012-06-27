#-------------------------------------------------
#
# Project created by QtCreator 2012-06-15T16:09:39
#
#-------------------------------------------------

QT       += core
QT       += testlib


TARGET = serial_app_tst
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    tst_position.cpp\
    ../../src/servoboard/position.cpp \
    tst_sequence.cpp \
    ../../src/servoboard/sequence.cpp


HEADERS += \
    tst_position.h\
    ../../src/servoboard/position.h \
    tst_sequence.h \
    ../../src/servoboard/sequence.h



INCLUDEPATH += ../../src/servoboard
