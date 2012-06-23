#define QT_NO_DEBUG_OUTPUT

#include <QtCore/QCoreApplication>
#include <QTest>
#include <QtCore/QDebug>



#ifndef __PRETTY_FUNCTION__
 #ifndef __FUNCDNAME__
  #define __PRETTY_FUNCTION__ __FILE__
 #endif//#ifndef __FUNCDNAME__
 #ifdef __FUNCDNAME__
  #define __PRETTY_FUNCTION__ __FUNCDNAME__
 #endif //#ifdef __FUNCDNAME__
#endif //__PRETTY_FUNCTION__

#include "tst_position.h"
#include <iostream>
#include <QFile>
#include <QTextStream>

QTextStream s;
void myMessageOutput(QtMsgType type, const char *msg)
{
}

int main(int argc, char *argv[])
{
    tst_Position p;
    QTest::qExec(&p, argc, argv);

}
