#include "connectioncontroller.h"
/*Constructors*/
ConnectionController::ConnectionController(QObject *parent) :
    QObject(parent), port(0), enumerator(0)
{
    this->init();
}
ConnectionController::ConnectionController(QObject* parent, SerialWidget* view, AbstractSerial* sPort) :
    QObject(parent), port(sPort), enumerator(0), view(view)
{
    this->init();
}
ConnectionController::~ConnectionController()
{
    /*if(this->port)
    {
        port->close();
        delete port;
    }*/
}

void ConnectionController::init()
{
    this->initEnumerator();
    this->initSerial();
    this->initView();

}

/*Public methods*/
AbstractSerial* ConnectionController::getSerialPort()
{
    return this->port;
}

void ConnectionController::RTSToggle()
{
    if(this->port && this->port->isOpen())
    {
        bool t = AbstractSerial::LineRTS & this->port->lineStatus();
        this->port->setRts(!t);
        view->updateSerialLineStates(this->port->lineStatus(),true);
    }

}
void ConnectionController::DTRToggle()
{
    if(this->port && this->port->isOpen())
    {
        bool t = AbstractSerial::LineDTR & this->port->lineStatus();
        this->port->setDtr(!t);
        view->updateSerialLineStates(this->port->lineStatus(),true);
    }

}
void ConnectionController::changeConnectionParameters(QString baudrate,
                                                      QString databits,
                                                      QString parity,
                                                      QString stopbits,
                                                      QString flow)
{
    if (this->port && this->port->isOpen()) {
        QStringList notApplyList;
        QString setting;
        bool result = true;

        setting = baudrate;
        if ((this->port->baudRate() != setting) && (!this->port->setBaudRate( setting ))) {
            // failed to apply
            notApplyList << setting;
            result = false;
        }

        setting =databits;
        if ((this->port->dataBits() != setting) && (!this->port->setDataBits( setting ))) {
            // failed to apply
            notApplyList << setting;
            result = false;
        }

        setting = parity;
        if ((this->port->parity() != setting) && (!this->port->setParity( setting ))) {
            // failed to apply
            notApplyList << setting;
            result = false;
        }

        setting = stopbits;
        if ((this->port->stopBits() != setting) && (!this->port->setStopBits( setting ))) {
            // failed to apply
            notApplyList << setting;
            result = false;
        }

        setting = flow;
        if ((this->port->flowControl() != setting) && (!this->port->setFlowControl(setting))) {
            // failed to apply
            notApplyList << setting;
            result = false;
        }

        if (!result) {
            QString notApplStr;
            foreach (QString s, notApplyList) {
                notApplStr.append(QString("\n %1").arg(s));
            }
            qDebug() << "Failed on the following settings: " << notApplStr;

        }
    }
}

void ConnectionController::open(QString port)
{
    if (!this->port)
    {
        this->initSerial();
    }
    this->port->setDeviceName(port);
    if (!this->port->open(QIODevice::ReadWrite))
    {
        qDebug() << "Port failed to open in connectControl";
    }

    this->port->setBaudRate("9600 baud");
    this->port->setDataBits("8 bit");
    this->port->setParity("None");
    this->port->setStopBits("1");
    this->port->setFlowControl("Disable");
    view->initSerialWidgetOpenState(this->port->lineStatus());
    QStringList baudrates = this->port->listBaudRate();
    QStringList data = this->port->listDataBits();
    QStringList parity = this->port->listParity();
    QStringList stop  = this->port->listStopBits();
    QStringList flow = this->port->listFlowControl();
    view->updateOptionsWidget(baudrates,data,parity,stop,flow);
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
    if (!this->port)
    {
        port = new AbstractSerial();//don't want to use this as parent since it won't live as long
    }
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
    if (this->port && this->port->isOpen())
    {
        view->initSerialWidgetOpenState(this->port->lineStatus());
    }
    else
    {
        view->initSerialWidgetCloseState(0);
    }

    QStringList devices = this->enumerator->devicesAvailable();
    view->updateEnumeratedDevices(devices);
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
void ConnectionController::sendSerialData(QByteArray data)
{
    if(this->port && this->port->isOpen())
    {
        this->port->write(data);
    }
}

