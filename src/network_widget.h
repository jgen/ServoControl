#ifndef NETWORK_WIDGET_H
#define NETWORK_WIDGET_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QUdpSocket;
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


private slots:
    void processPendingDatagrams();
    void on_btnListen_clicked();

private:
    unsigned long stats_packets;
    unsigned long stats_invalid;
    bool socketOpen;
    QUdpSocket *udpSocket;    
    Ui::NetworkWidget *ui;
};

#endif // NETWORK_WIDGET_H
