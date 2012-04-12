#ifndef SERIALWRAPPER_H
#define SERIALWRAPPER_H

#include <QObject>
#include <QDebug>
#include <serialdeviceenumerator.h>
#include <abstractserial.h>


typedef AbstractSerial::BaudRate BaudRate;
typedef AbstractSerial::Parity Parity;
typedef AbstractSerial::Flow Flow;
typedef AbstractSerial::DataBits DataBits;
typedef AbstractSerial::StopBits StopBits;

class SerialWrapper : public QObject
{
    Q_OBJECT
   /* Q_PROPERTY(BaudRate baudRate READ getBaudRate WRITE setBaudRate);
    Q_PROPERTY(Parity parity READ getParity WRITE setParity);
    Q_PROPERTY(Flow flowControl READ getFlow WRITE setFlow);
    Q_PROPERTY(DataBits dataBits READ getDataBits WRITE setDataBits);
    Q_PROPERTY(StopBits stopBits READ getStopBits WRITE setStopBits);*/

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
    void setDTR(bool);

    bool setBaudRate(QString baudrate){return this->port->setBaudRate(baudrate);}
    QString getBaudRate(){return this->baudRate;}

    bool setParity(QString parity){return this->port->setParity(parity);}
    QString getParity(){return this->parity;}

    bool setFlow(QString flow){return this->port->setFlowControl(flow);}
    QString getFlow(){return this->flowControl;}

    bool setDataBits(QString databits){return this->port->setDataBits(databits);}
    QString getDataBits(){return this->dataBits;}

    bool setStopBits(QString stopBits){return this->port->setStopBits(stopBits);}
    QString getStopBits(){return this->stopBits;}



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
    QString baudRate;
    QString parity;
    QString flowControl;
    QString stopBits;
    QString dataBits;
    AbstractSerial* port;


};

#endif // SERIALWRAPPER_H
