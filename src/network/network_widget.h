#ifndef NETWORK_WIDGET_H
#define NETWORK_WIDGET_H

#include <QWidget>
#include <QQueue>

QT_BEGIN_NAMESPACE
class QUdpSocket;
class QTimer;
class QLineEdit;
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
    void setPort(AbstractSerial* port);

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
    void on_btnClearEmergencyStop_clicked();

private:
    void emptyQueue();

    unsigned long stats_packets;
    unsigned long stats_packets_old;
    unsigned long stats_invalid;
    unsigned long stats_queue_max;
    unsigned long stats_discarded;
    unsigned long stats_network_failures;

    bool socketOpen;
    unsigned int portNum, outputTime, networkFailureTime; // UDP port number, Time in ms for the timers.
    unsigned char defaultServoValue;
    // This array holds the current value for each of the 12 servos.
    unsigned char servo[12];
    // For PID control
    float gain_p, gain_d, gain_i;
	float servo_integral[12];
    int servo_error[12];

    // This array points to the LineEdit boxes on the form for easy updating.
    QLineEdit *currentServoValue[12];

    AbstractSerial *serial;
    QQueue<QByteArray> queue;
    QTimer *OutputTimer, *NetworkTimer;
    QUdpSocket *udpSocket;
    Ui::NetworkWidget *ui;
};

#endif // NETWORK_WIDGET_H
