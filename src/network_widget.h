#ifndef NETWORK_WIDGET_H
#define NETWORK_WIDGET_H

#include <QWidget>
#include <QQueue>

QT_BEGIN_NAMESPACE
class QUdpSocket;
class QTimer;
QT_END_NAMESPACE

namespace Ui {
    class NetworkWidget;
}

class NetworkWidget : public QWidget
{
    Q_OBJECT

public:
    explicit NetworkWidget(QWidget *parent = 0);
    ~NetworkWidget();
    bool validPacket(const char *);

public slots:
    void processQueue();
    void processPendingDatagrams();

private slots:
    void on_btnListen_clicked();

private:
    unsigned long stats_packets;
    unsigned long stats_invalid;
    bool socketOpen;
    QQueue<QByteArray> queue;
    QTimer *timer;
    QUdpSocket *udpSocket;
    Ui::NetworkWidget *ui;
};

#endif // NETWORK_WIDGET_H
