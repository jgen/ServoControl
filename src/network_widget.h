#ifndef NETWORK_WIDGET_H
#define NETWORK_WIDGET_H

#include <QWidget>
#include <QQueue>

QT_BEGIN_NAMESPACE
class QUdpSocket;
class QTimer;
class AbstractSerial;
QT_END_NAMESPACE

namespace Ui {
    class NetworkWidget;
}

class NetworkWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkWidget(QWidget *parent = 0, AbstractSerial *serial = NULL);
    ~NetworkWidget();
    bool validPacket(const char *);

    // State of Network Widget
    enum NetworkWidgetState { NOT_LISTENING, LISTENING, NET_FAILURE, COM_FAILURE, EMERG_STOP };
    enum NetworkWidgetState STATE;

public slots:
    void ouputServoData();
    void sendDefaults();
    void checkNetwork();
    void processPendingDatagrams();

private slots:
    void on_btnListen_clicked();

private:
    unsigned long stats_packets;
    unsigned long stats_invalid;
    unsigned long stats_queue_max;
    unsigned long stats_discarded;

    bool socketOpen;
    unsigned int outputTime, networkFailureTime; // Time in ms for the timers
    unsigned char defaultServoValue;
    // This array holds the current value for each of the 12 servos
    unsigned char servo[12];

    AbstractSerial *serial;
    QQueue<QByteArray> queue;
    QTimer *OutputTimer, *NetworkTimer;
    QUdpSocket *udpSocket;
    Ui::NetworkWidget *ui;
};

#endif // NETWORK_WIDGET_H
