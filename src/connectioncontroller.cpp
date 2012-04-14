#include "connectioncontroller.h"
/*Constructors*/
ConnectionController::ConnectionController(QObject *parent) :
    QObject(parent), port(0), enumerator(0)
{
}
ConnectionController::ConnectionController(QObject* parent, SerialWidget* view) :
    QObject(parent), port(0), enumerator(0), view(view)
{

}
/*Public methods*/
void ConnectionController::open(QString port)
{
    if (!this->port)
    {
        this->initSerial();
    }
    this->port->setDeviceName(port);
    this->port->open(QIODevice::ReadWrite);
    view->initSerialWidgetOpenState(this->port->lineStatus());


}


void ConnectionController::close()
{
    port->close();
    view->initSerialWidgetCloseState(this->port->lineStatus());
}
void ConnectionController::updateInfoWidget(QString name)
{
    if(this->enumerator)
    {
        view->updateInfoData(enumerator,name );
    }
}

/*private methods*/
void ConnectionController::initEnumerator()
{
    if (!this->enumerator)
        this->enumerator = SerialDeviceEnumerator::instance();
    connect(this->enumerator, SIGNAL(hasChanged(QStringList)), this, SLOT(serialDevicesChanged(QStringList)));
    view->updateEnumeratedDevices(this->enumerator->devicesAvailable());
}
void ConnectionController::initSerial()
{
    if (this->port)
        return;
    port = new AbstractSerial();//don't want to use this as parent since it won't live as long
    connect(this->port, SIGNAL(signalStatus(QString,QDateTime)),
            this, SLOT(recieveSerialMessages(QString,QDateTime)));
    connect(this->port, SIGNAL(ctsChanged(bool)), this, SLOT(ctsChanged(bool)));
    connect(this->port, SIGNAL(dsrChanged(bool)), this, SLOT(dsrChanged(bool)));
    connect(this->port, SIGNAL(ringChanged(bool)), this, SLOT(ringChanged(bool)));
    connect(this->port, SIGNAL(readyRead()), this, SLOT(readyToRead()));

    // Enable emmiting signal signalStatus
    this->port->enableEmitStatus(true);

    this->port->setBaudRate("9600 baud");
    this->port->setDataBits("8 bit");
    this->port->setParity("None");
    this->port->setStopBits("1");
    this->port->setFlowControl("Disable");


}
void ConnectionController::initView()
{
    connect(this,SIGNAL(CTSChanged(bool)),view, SLOT(procCtsChanged(bool)));
    connect(this,SIGNAL(DSRChanged(bool)),view,SLOT(procCtsChanged(bool)));
    connect(this,SIGNAL(RingChanged(bool)),view,SLOT(procRingChanged(bool)));
}
void ConnectionController::deinitSerial()
{
    if (this->port && this->port->isOpen())
        this->port->close();
}

/*Private slots*/
void ConnectionController::serialDevicesChanged(QStringList list)
{
    view->updateEnumeratedDevices(list);
}
void ConnectionController::recieveSerialMessages(QString msg, QDateTime dt)
{
    qDebug() << msg;
}
void ConnectionController::ctsChanged(bool value)
{
    emit CTSChanged(value);
}
void ConnectionController::dsrChanged(bool value)
{
    emit DSRChanged(value);
}
void ConnectionController::ringChanged(bool value)
{
    emit RingChanged(value);
}
void ConnectionController::readyToRead()
{
    QByteArray data = this->port->readAll();
    qDebug() << "Rx: " << data;
    view->printToTrace(data,true);
}
bool ConnectionController::sendData(QByteArray data)
{
    if (this->port && this->port->isOpen())
    {
        this->port->write(data);
        return true;
    }
    else
    {
        return false;
    }
}

