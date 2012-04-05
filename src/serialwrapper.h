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

public slots:

private:
    AbstractSerial* port;
    QString deviceName;


};

#endif // SERIALWRAPPER_H
