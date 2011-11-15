#include <QString>
#include <QQueue>
#include <QtNetwork>
#include <QTimer>
#include <QtCore/QDebug>
#include <QMessageBox>

#include "network_widget.h"
#include "ui_network_widget.h"

NetworkWidget::NetworkWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::NetworkWidget)
{
    ui->setupUi(this);
    udpSocket = new QUdpSocket(this);

    if ( udpSocket->localAddress().isNull() ) {
        ui->lblIP_address->setText("0.0.0.0");
    } else {
        ui->lblIP_address->setText( udpSocket->localAddress().toString() );
    }

    socketOpen = false;
    stats_packets = 0;
    stats_invalid = 0;
    ui->lblStatus->setText(tr("Not Listening"));

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(processQueue()));
}

NetworkWidget::~NetworkWidget()
{
    if (udpSocket->isOpen()) {
        udpSocket->close();
    }
    queue.clear();
    timer->stop();

    delete udpSocket;
    delete timer;
    delete ui;
}

/*
    We should get the datagrams and store them in a queue of sorts.
    Have a QTimer that is running and taking them out of the queue.
*/
void NetworkWidget::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        // Increment the received packet count
        stats_packets++;

        // check if valid packet then
        if (validPacket(datagram.constData())) {
            queue.enqueue(datagram);
        }

        //statusLabel->setText(tr("Received datagram: \"%1\"").arg(datagram.data()));
    }
}

bool NetworkWidget::validPacket(const char * packet)
{
    QByteArray data(packet);

    if (data.size() == 0 || data.size() > 25) {
        return false;
    }

    unsigned char pairs = data[0] * 2;

    if (pairs > 12) {
        return false;
    }

    for (unsigned int i=1; i <= pairs; i+=2) {
        if (data[i] > 12) {
            return false;
        }
        if (data[i+1] < 1 || data[i+1] > 127) {
            return false;
        }
    }

    return true;
}

void NetworkWidget::processQueue()
{
    // handle items that have been received from the network.
    QByteArray message;
    unsigned char servo, value, pairs;

    if (queue.isEmpty()) {
        return;
    }

    message = queue.dequeue();

    if (message[0] == 0) {
        // STOP CONDITION

        // output whatever is need for stop.
        return;
    }

    pairs = (message[0] * 2);

    for (unsigned int i=1; i <= pairs; i+=2) {
        servo = message[i];
        value = message[i+1];
        // Send off to the Servo board. (Another timer needed?)
    }
}

void NetworkWidget::on_btnListen_clicked()
{
    unsigned int portNum = 0;
    bool convert = false;
    QString status;

    if (socketOpen){
        // Stop Listening
        udpSocket->close();
        timer->stop();
        queue.clear();
        socketOpen = false;
        qDebug() << "Network: Closing socket.";
        ui->btnListen->setText(tr("Listen"));
        ui->lblStatus->setText(tr("Not Listening"));

    } else {
        // Start Listening
        portNum = ui->txtPortNumber->text().toInt(&convert, 10);

        if (!convert) {
            // Error occured converting the port number.
            // pop up an error message.
            QMessageBox::critical(this, tr("Invalid Port Number"),
                                        tr("The port number you entered is invalid.\n"
                                           "Please enter a valid number."),
                                           QMessageBox::Ok);
            return;
        }

        queue.clear();
        udpSocket->bind( udpSocket->localAddress(), portNum, QUdpSocket::ShareAddress );
        socketOpen = true;
        connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

        // This value should be user configurable
        timer->start(500);

        status = "Listening on " + udpSocket->localAddress().toString() + ":" + QString::number(portNum, 10);
        qDebug() << "Network: " << status << " UDP.";

        ui->lblStatus->setText(status);
        ui->btnListen->setText(tr("Stop"));
    }
}

