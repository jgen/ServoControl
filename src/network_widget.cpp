#include <QString>
#include <QtNetwork>
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
}

NetworkWidget::~NetworkWidget()
{
    if (udpSocket->isOpen()) {
        udpSocket->close();
    }

    delete udpSocket;
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
        stats_packets++;

        //statusLabel->setText(tr("Received datagram: \"%1\"").arg(datagram.data()));
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

        udpSocket->bind( udpSocket->localAddress(), portNum, QUdpSocket::ShareAddress );
        socketOpen = true;
        connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

        status = "Listening on " + udpSocket->localAddress().toString() + ":" + QString::number(portNum, 10);

        qDebug() << "Network: " << status << " UDP.";

        ui->lblStatus->setText(status);
        ui->btnListen->setText(tr("Stop"));
    }
}
