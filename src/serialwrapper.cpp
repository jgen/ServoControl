#include "serialwrapper.h"
#include <QStringList>

SerialWrapper::SerialWrapper(QObject *parent, AbstractSerial *p) :
        QObject(parent), port(p) , deviceName("")
{
}

bool SerialWrapper::isConnected()
{
    return port->isOpen();
}
void SerialWrapper::close()
{
    this->port->close();
    return;
}
void SerialWrapper::setDeviceName(QString name)
{
    port->setDeviceName(name);
    this->deviceName= name;
}
QString SerialWrapper::getDeviceName()
{
    return deviceName;
}
bool SerialWrapper::open(QIODevice::OpenModeFlag flags)
{
    return port->open(flags);
}
bool SerialWrapper::serialConnect()
{
    return port->open(QIODevice::ReadWrite);
}
bool SerialWrapper::serialConnect(QString name)
{
    this->setDeviceName(name);
    return this->serialConnect();
}
QStringList SerialWrapper::listBaudRate()
{
    return port->listBaudRate();
}
QStringList SerialWrapper::listDataBits()
{
    return port->listDataBits();
}
QStringList SerialWrapper::listFlowControl()
{
    return port->listFlowControl();
}
QStringList SerialWrapper::listParity()
{
    return port->listParity();
}
QStringList SerialWrapper::listStopBits()
{
    return port->listStopBits();
}
