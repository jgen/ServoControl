#include "serialwrapper.h"
#include <QStringList>

SerialWrapper::SerialWrapper(QObject *parent, AbstractSerial *p) :
        QObject(parent), port(p) , deviceName(""),baudRate(AbstractSerial::BaudRate9600),
        dataBits(AbstractSerial::DataBits8),parity(AbstractSerial::ParityNone),
        flowControl(AbstractSerial::FlowControlOff), stopBits(AbstractSerial::StopBits1)
{
    this->initalize();
}

void SerialWrapper::initalize()
{
    connect(this->port, SIGNAL(signalStatus(QString,QDateTime)), this, SLOT(recSignalStatus(QString,QDateTime)));
    connect(this->port, SIGNAL(ctsChanged(bool)), this, SLOT(recCtsChanged(bool)));
    connect(this->port, SIGNAL(dsrChanged(bool)), this, SLOT(recDsrChanged(bool)));
    connect(this->port, SIGNAL(ringChanged(bool)), this, SLOT(recRingChanged(bool)));
    connect(this->port, SIGNAL(readyRead()), this, SLOT(recReadyRead()));
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
void SerialWrapper::recSignalStatus(QString string, QDateTime time)
{
    emit signalStatus(string, time);
}
void SerialWrapper::recCtsChanged(bool flag)
{
    emit ctsChanged(flag);
}
void SerialWrapper::recDsrChanged(bool flag)
{
    emit dsrChanged(flag);
}
void SerialWrapper::recRingChanged(bool flag)
{
    emit ringChanged(flag);
}
void SerialWrapper::recReadyRead()
{
    emit readyRead();
}
quint16 SerialWrapper::lineStatus()
{
    return this->port->lineStatus();
}
QByteArray SerialWrapper::readAll()
{
    return this->port->readAll();
}
void SerialWrapper::write(const QByteArray& data)
{
    port->write(data);
}
void SerialWrapper::setRTS(bool value)
{
    this->port->setRts(value);
}
void SerialWrapper::setDTR(bool value)
{
    this->port->setDtr(value);
}
