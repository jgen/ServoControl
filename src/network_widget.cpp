#include <QString>
#include <QQueue>
#include <QtNetwork>
#include <QTimer>
#include <QtCore/QDebug>
#include <QMessageBox>

#include <abstractserial.h>

#include "network_widget.h"
#include "ui_network_widget.h"

NetworkWidget::NetworkWidget(QWidget *parent, AbstractSerial *serial) :
    QWidget(parent),
    serial(serial),
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
    STATE = NOT_LISTENING;

    // Zero out all the stats.
    stats_packets = stats_invalid = stats_queue_max = stats_discarded=0;
    ui->lblStatus->setText(tr("Not Listening"));

    OutputTimer = new QTimer(this);
    NetworkTimer = new QTimer(this);
    connect(OutputTimer, SIGNAL(timeout()), this, SLOT(ouputServoData()));
    connect(NetworkTimer, SIGNAL(timeout()), this, SLOT(checkNetwork()));
}

NetworkWidget::~NetworkWidget()
{
    OutputTimer->stop();
    NetworkTimer->stop();

    if (udpSocket->isOpen()) udpSocket->close();

    queue.clear();

    delete udpSocket;
    delete OutputTimer;
    delete NetworkTimer;
    delete ui;
}

/*
    Get the datagrams and store them in a queue.
    We have a QTimer that is running and taking them out of the queue.
*/
void NetworkWidget::processPendingDatagrams()
{
    while (udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(datagram.data(), datagram.size());

        if (STATE == EMERG_STOP) {
            // Discard all packets
            return;
        }

        // Increment the received packet count
        stats_packets++;

        // check if valid packet
        if (validPacket(datagram.constData())) {
            queue.enqueue(datagram);
        } else {
            stats_invalid++;
        }
    }

    if (NetworkTimer->isActive()){
        // Restart the Timer
        NetworkTimer->stop();
        NetworkTimer->start( networkFailureTime );
    }
}

/*
  Determine if a given packet is valid.

  Returns TRUE if valid.
  Returns FALSE if there is a problem.
*/
bool NetworkWidget::validPacket(const char * packet)
{
    QByteArray data(packet);

    if (data.size() == 0 || data.size() > 25)
        return false;

    unsigned char pairs = data[0] * 2;

    if (pairs > 12)
        return false;

    for (unsigned int i=1; i <= pairs; i+=2) {
        if (data[i] > 12)
            return false;
        if (data[i+1] < 1 || data[i+1] > 127)
            return false;
    }

    return true;
}

// Output data to the ServoBoard
void NetworkWidget::ouputServoData()
{
    if (STATE == NET_FAILURE || STATE == EMERG_STOP){
        sendDefaults();
        return;
    }

    if (STATE == COM_FAILURE || !this->serial || !this->serial->isOpen()){
        // communications failure
        STATE = COM_FAILURE;
        // Should we try to get the COM port back?
        return;
    }

    // handle items that have been received from the network.
    QByteArray message, data(2,0);
    unsigned char pairs;

    if (queue.isEmpty())
        return;

    message = queue.dequeue();

    if (ui->chkSendMostRecent->isChecked()) {
        // dump the queue
        queue.clear();
    }

    if (message[0] == 0) {
        // STOP CONDITION
        STATE = EMERG_STOP;

        // Stop the network timer
        NetworkTimer->stop();

        // output whatever is need for stop.
        sendDefaults();
        return;
    }

    pairs = (message[0] * 2);

    for (unsigned int i=1; i <= pairs; i+=2) {
        data[0] = 144 + message[i];     // servo number
        data[1] = message[i+1];         // servo value

        // Send off to the Servo board.
        if (this->serial->write( data ) == -1) {
            STATE = COM_FAILURE;
            return;
        }
    }
}

void NetworkWidget::sendDefaults()
{
    if (STATE == COM_FAILURE){
        return;
    }
    if (!this->serial || !this->serial->isOpen()){
        // communications failure
        STATE = COM_FAILURE;
        return;
    }

    QByteArray data(2, defaultServoValue);

    for (unsigned char i=0; i<12; i++) {
        data[0] = 144 + i; // 144 = 0b1001_0000

        if (this->serial->write( data ) == -1) {
            STATE = COM_FAILURE;
            return;
        }
    }
}

void NetworkWidget::checkNetwork()
{
    // check if we have received any packets since last time.
    static unsigned long packets_received = 0;

    if (stats_packets == 0 || stats_packets > packets_received) {
        // Restart the Timer
        NetworkTimer->stop();
        NetworkTimer->start( networkFailureTime );

        packets_received = stats_packets; // Update the value
        return;
    }

    if (stats_packets == packets_received) {
        // Network timeout has occured.
        ui->lblStatus->setText("Network Failure!");
        STATE = NET_FAILURE;

        NetworkTimer->stop();
        // Dump queue, and send defaults.
        queue.clear();
        sendDefaults();
    }
}

void NetworkWidget::on_btnListen_clicked()
{
    unsigned int portNum;
    bool convert = false;
    QString status;

    if (socketOpen){
        // Stop Listening
        udpSocket->close();
        OutputTimer->stop();
        NetworkTimer->stop();
        queue.clear();
        socketOpen = false;
        qDebug() << "Network: Closing socket.";
        ui->btnListen->setText(tr("Listen"));
        ui->lblStatus->setText(tr("Not Listening"));
        ui->grpOptions->setEnabled(true);
        STATE = NOT_LISTENING;
    } else {
        // Start Listening
        portNum = ui->txtPortNumber->text().toInt(&convert, 10);
        if (!convert) {
            QMessageBox::critical(this, tr("Invalid Port Number"),tr("The number you entered is invalid.\n Please enter a valid number."),QMessageBox::Ok);
            return;
        }
        defaultServoValue = ui->lineDefaultValue->text().toInt(&convert, 10);
        if (!convert) {
            QMessageBox::critical(this, tr("Invalid Default Number"),tr("The number you entered is invalid.\n Please enter a valid number."),QMessageBox::Ok);
            return;
        }
        outputTime = ui->lineOutputRateMs->text().toInt(&convert, 10);
        if (!convert) {
            QMessageBox::critical(this, tr("Invalid Output Time Number"),tr("The number you entered is invalid.\n Please enter a valid number."),QMessageBox::Ok);
            return;
        }
        networkFailureTime = ui->lineOutputRateMs->text().toInt(&convert, 10);
        if (!convert) {
            QMessageBox::critical(this, tr("Invalid Network Failure Number"),tr("The number you entered is invalid.\n Please enter a valid number."),QMessageBox::Ok);
            return;
        }
        ui->grpOptions->setDisabled(true);

        queue.clear();
        udpSocket->bind( udpSocket->localAddress(), portNum, QUdpSocket::ShareAddress );
        socketOpen = true;
        connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

        // This value should be user configurable
        /*
          Maximum Data throughput on 9600 baud is 960 bytes/sec.

          Since we need to send 2 bytes each time, we can only output to the
          servo board at a max rate of 480 times/sec.

          1/480 = approx 2 ms.

          Thus the absolute max rate of sending data to the servo board is once every 2 ms.
        */
        OutputTimer->start( outputTime );

        status = "Listening on " + udpSocket->localAddress().toString() + ":" + QString::number(portNum, 10);
        qDebug() << "Network: " << status << " UDP.";

        ui->lblStatus->setText(status);
        ui->btnListen->setText(tr("Stop"));
        STATE = LISTENING;
    }
}

