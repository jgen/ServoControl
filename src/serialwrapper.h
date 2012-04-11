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
    quint16 lineStatus();
    QByteArray readAll();
    void write(const QByteArray& data);
    QStringList listBaudRate();
    QStringList listDataBits();
    QStringList listParity();
    QStringList listStopBits();
    QStringList listFlowControl();

    void setRTS(bool);

    enum LineStatusFlag {
        LineLE    = 0x0001, /*!< \~english Line DSR (data set ready/line enable). */
        LineDTR   = 0x0002, /*!< \~english Line DTR (data terminal ready). */
        LineRTS   = 0x0004, /*!< \~english Line RTS (request to send). */
        LineST    = 0x0008, /*!< \~english Line Secondary TXD (transmit). */
        LineSR    = 0x0010, /*!< \~english Line Secondary RXD (receive.) */
        LineCTS   = 0x0020, /*!< \~english Line CTS (clear to send). */
        LineDCD   = 0x0040, /*!< \~english Line DCD (data carrier detect). */
        LineRI    = 0x0080, /*!< \~english Line RNG (ring). */
        LineDSR   = 0x0100, /*!< \~english Line DSR (data set ready). */

        LineErr   = 0x8000, /*!< \~english Error get line status. */
    };
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
