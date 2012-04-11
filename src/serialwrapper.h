#ifndef SERIALWRAPPER_H
#define SERIALWRAPPER_H

#include <QObject>
#include <QDebug>
#include <serialdeviceenumerator.h>
#include <abstractserial.h>



class SerialWrapper : public QObject
{
    Q_OBJECT
public:
    explicit SerialWrapper(QObject *parent = 0, AbstractSerial* p = 0);

    bool isConnected();
    void close();
    void setDeviceName(QString name);
    QString getDeviceName();
    bool open(QIODevice::OpenModeFlag flags);
    bool serialConnect(QString name);
    bool serialConnect();
    QStringList listBaudRate();
    QStringList listDataBits();
    QStringList listParity();
    QStringList listStopBits();
    QStringList listFlowControl();


signals:
    void signalStatus(QString, QDateTime);
    void ctsChanged(bool);
    void dsrChanged(bool);
    void ringChanged(bool);
    void readyRead();
public slots:

private slots:
    void recSignalStatus(QString, QDateTime);
    void recCtsChanged(bool);
    void recDsrChanged(bool);
    void recRingChanged(bool);
    void recReadyRead();
private:
    void initalize();

    QString deviceName;
    AbstractSerial::BaudRate baudRate;
    AbstractSerial::Parity parity;
    AbstractSerial::Flow flowControl;
    AbstractSerial::StopBits stopBits;
    AbstractSerial::DataBits dataBits;
    AbstractSerial* port;


};

#endif // SERIALWRAPPER_H
