#ifndef CONNECTIONCONTROLLER_H
#define CONNECTIONCONTROLLER_H

#include <QObject>
#include <QDebug>
#include <serialdeviceenumerator.h>
#include <abstractserial.h>

#include "serialwrapper.h"
#include "serial_widget.h"


class ConnectionController : public QObject
{
    Q_OBJECT
public:
    explicit ConnectionController(QObject* parent, SerialWidget* view, AbstractSerial* port);
    ~ConnectionController();

    void open(QString port);
    void close();
    bool sendData(QByteArray data);
    void updateInfoWidget(QString name);
    void RTSToggle();
    void DTRToggle();
    void changeConnectionParameters(QString baudrate, QString databits, QString parity,
                                    QString stopbits, QString flow);
    AbstractSerial* getSerialPort();

signals:
    void CTSChanged(bool);
    void DSRChanged(bool);
    void RingChanged(bool);

public slots:
    void sendSerialData(QByteArray data);
private slots:
    void serialDevicesChanged(QStringList);
    void recieveSerialMessages(QString,QDateTime);
    void ctsChanged(bool);
    void dsrChanged(bool);
    void ringChanged(bool);
    void readyToRead();

private:
    explicit ConnectionController(){} //No constuction unless it is through the right constuctors
    explicit ConnectionController(QObject *parent = 0);

    void init();

    void initEnumerator();
    void initSerial();
    void initView();

    void deinitSerial();

    AbstractSerial* port;
    SerialDeviceEnumerator* enumerator;
    SerialWidget* view;

};

#endif // CONNECTIONCONTROLLER_H
