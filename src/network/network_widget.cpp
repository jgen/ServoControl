/*

 This widget receives servo commands from the network.

 This was used to control the temporary WE First robot that was built
 using a laptop for control.
*/

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
    connect(udpSocket, SIGNAL(readyRead()), this, SLOT(processPendingDatagrams()));

    if ( udpSocket->localAddress().isNull() ) {
        ui->lblIP_address->setText("0.0.0.0");
    } else {
        ui->lblIP_address->setText( udpSocket->localAddress().toString() );
    }

    QLineEdit *temp[] = { ui->PWM_01, ui->PWM_02, ui->PWM_03, ui->PWM_04, ui->PWM_05, ui->PWM_06, ui->PWM_07, ui->PWM_08, ui->PWM_09, ui->PWM_10, ui->PWM_11, ui->PWM_12 };
    for (unsigned char i=0; i<(sizeof(temp)/sizeof(temp[0])); i++)
        currentServoValue[i] = temp[i];

    // Zero out all the stats.
    stats_packets = stats_invalid = stats_queue_max = stats_discarded = stats_packets_old = stats_network_failures = 0;
    ui->lblStatus->setText(tr("Not Listening"));

    socketOpen = false;
    STATE = NOT_LISTENING;
    emptyQueue();

    ui->btnClearEmergencyStop->setDisabled(true);

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

        if (STATE == EMERG_STOP || (STATE == NET_FAILURE && ui->chkStopOnNetworkFailure->isChecked())) {
            // Discard all packets
            return;
        }

        // STOP CONDITION
        if ( datagram.size() && datagram[0]==(char)0 ) {
            STATE = EMERG_STOP;

            // output whatever is need for stop.
            sendDefaults();

            // Stop the network timer
            NetworkTimer->stop();
            emptyQueue();

            ui->btnClearEmergencyStop->setEnabled(true);
            ui->lblEmergencyStatus->setText("EMERGENCY STOP!");
            QPalette red_text;
            red_text.setColor(QPalette::WindowText, Qt::red);
            ui->lblEmergencyStatus->setPalette(red_text);
            ui->lblEmergencyStatus->setFrameStyle(QFrame::Box);
            ui->lblStatus->setText("EMERGENCY STOP!");
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

    // Update the Statistics
    ui->lblPackets->setText(QString::number(stats_packets,10));
    ui->lblInvalid->setText(QString::number(stats_invalid,10));

    // Check if a network failure occured.
    if (STATE == NET_FAILURE && !ui->chkStopOnNetworkFailure->isChecked()) {
        STATE = LISTENING;
        NetworkTimer->start( networkFailureTime );
    }
    ui->lblStatus->setText("Receiving Packets...");
}

/*
  Determine if a given packet is valid.

  Returns TRUE if valid.
  Returns FALSE if there is a problem.
*/
bool NetworkWidget::validPacket(const char * packet)
{
    QByteArray data(packet);

    if ( (data.size() < 1 || data.size() > 25) || (data[0] > (char)12) )
        return false;

    unsigned char pairs = data[0] * 2;

    if ( (pairs+1) > data.size() )
        return false;

    for (unsigned int i=1; i <= pairs; i+=2) {
        if (data[i] > (char)12)
            return false;
        if (data[i+1] < (char)1 || data[i+1] > (char)127)
            return false;
    }

    return true;
}

// Output data to the ServoBoard
void NetworkWidget::ouputServoData()
{
    if (STATE == NET_FAILURE || STATE == EMERG_STOP) {
        emptyQueue();
        if (!ui->chkDefaultsOnNetworkFail->isChecked()) {
            return;
        }
        sendDefaults();
        return;
    }

    if (STATE == COM_FAILURE || !this->serial || !this->serial->isOpen()){
        // communications failure
        STATE = COM_FAILURE;
        // Should we try to get the COM port back?
        return;
    }

    if (queue.isEmpty())
        return;

    // handle items that have been received from the network.
    QByteArray message, data(2,0);
    unsigned char pairs, current_servo;
    int error; float p_term, d_term;

    // Update the statistics on the Max Queue size
    bool convert;
    int max = ui->lblQueueMax->text().toInt(&convert, 10);
    if (convert && max < queue.size())
        ui->lblQueueMax->setText(QString::number(queue.size(), 10));

    do {
        message = queue.dequeue();
    } while (message.size() < 1);

    if (ui->chkSendMostRecent->isChecked()) {
        // dump the queue
        emptyQueue();
    }

    gain_p = 0.26;
    gain_d = 0.04;
    gain_i = 0.015;
	
    pairs = (message[0] * 2);

    unsigned int i = 1;

    while (i < pairs) {
        current_servo = message[i] - 1;         // Store the current servo number. (range of current_servo: 0-11)
        data[0] = 144 + current_servo + 1;      // Output to Servo Board. (range of data: 1-12)

        if (ui->chkPIDcontrol->isChecked()) {
            // Basic PID control
            error = message[i+1] - servo[current_servo];    // error = new_value - old_value
            p_term = gain_p * error;
            servo_integral[current_servo] += gain_i * error;
            d_term = gain_d * (error - servo_error[current_servo])/(outputTime/100);
            servo[current_servo] = p_term + d_term + servo_integral[current_servo] + servo[current_servo];
            servo_error[current_servo] = error;

            // Maximum range exceeded?
            if (servo[current_servo] > 97) servo[current_servo] = 97;
            if (servo[current_servo] < 1)  servo[current_servo] = 1;

            data[1] = servo[current_servo];
        } else {
            // No control
            data[1] = servo[current_servo] = message[i+1];  // servo value
        }

        // Update the Current value on the form.
        currentServoValue[current_servo]->setText(QString::number(servo[current_servo],10));

        // Send off to the Servo board.
        if (this->serial->write( data ) == -1) {
            STATE = COM_FAILURE;
            return;
        }

        i += 2; // Go to the next servo pair
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
        servo[i] = data[0] = 144 + i;   // 144 = 0b1001_0000
        servo_error[i] = 0;

        // Update the Current value on the form.
        currentServoValue[i]->setText(QString::number(defaultServoValue,10));

        if (this->serial->write( data ) == -1) {
            STATE = COM_FAILURE;
            return;
        }
    }
}

void NetworkWidget::checkNetwork()
{
    // check if we have received any packets since last time.

    if (stats_packets == 0 || stats_packets > stats_packets_old) {
        // Restart the Timer
        NetworkTimer->stop();
        NetworkTimer->start( networkFailureTime );

        stats_packets_old = stats_packets; // Update the value
        return;
    }

    if (stats_packets == stats_packets_old) {
        // Network timeout has occured.
        STATE = NET_FAILURE;
        stats_network_failures++;
        ui->lblStatus->setText("Network Failure!");        
        ui->lblNetworkFailures->setText(QString::number(stats_network_failures, 10));

        NetworkTimer->stop();
        // Dump queue
        emptyQueue();

        if (ui->chkDefaultsOnNetworkFail->isChecked()) {
            sendDefaults();
        }
    }
}

void NetworkWidget::on_btnListen_clicked()
{
    bool convert = false;
    QString status;

    if (socketOpen){
        // ----- Stop Listening -----
        udpSocket->close();
        OutputTimer->stop();
        NetworkTimer->stop();
        emptyQueue();
        socketOpen = false;
        qDebug() << "Network: Closing socket.";
        ui->btnListen->setText(tr("Listen"));
        ui->lblStatus->setText(tr("Not Listening"));
        ui->grpOptions->setEnabled(true);
        STATE = NOT_LISTENING;
    } else {
        // ----- Start Listening -----
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
        networkFailureTime = ui->lineNetworkFailTimeMs->text().toInt(&convert, 10);
        if (!convert) {
            QMessageBox::critical(this, tr("Invalid Network Failure Number"),tr("The number you entered is invalid.\n Please enter a valid number."),QMessageBox::Ok);
            return;
        }
        ui->grpOptions->setDisabled(true);

        // Set the Default values.
        for (unsigned char i=0; i<12; i++) {
            servo[i] = defaultServoValue;
            servo_error[i] = 0;
            servo_integral[i] = 0;
            currentServoValue[i]->setText(QString::number(defaultServoValue,10));
        }

        /*
          Maximum Data throughput on 9600 baud is 960 bytes/sec.

          Since we need to send 2 bytes each time, we can only output to the
          servo board at a max rate of 480 times/sec.

          1/480 = approx 2 ms.

          Thus the absolute max rate of sending data to the servo board is once every 2 ms.
        */
        OutputTimer->start( outputTime );
        NetworkTimer->start( networkFailureTime );

        STATE = LISTENING;
        stats_packets_old = stats_packets = stats_discarded = 0; // Reset the packet count.
        emptyQueue();

        socketOpen = true;
        ui->btnListen->setText(tr("Stop"));

        // Open the Socket
        udpSocket->bind( udpSocket->localAddress(), portNum, QUdpSocket::ShareAddress );

        status = "Listening on " + udpSocket->localAddress().toString() + ":" + QString::number(portNum, 10);
        qDebug() << "Network: " << status << " UDP.";

        ui->lblStatus->setText(status);
    }
}

void NetworkWidget::on_btnClearEmergencyStop_clicked()
{
    if (STATE != EMERG_STOP) {
        return;
    }

    QString status;

    STATE = LISTENING;
    QPalette black_text;
    black_text.setColor(QPalette::WindowText, Qt::black);
    ui->lblEmergencyStatus->setPalette(black_text);
    ui->lblEmergencyStatus->setFrameStyle(QFrame::Plain);
    ui->lblEmergencyStatus->setText("Status: OK");

    status = "Listening on " + udpSocket->localAddress().toString() + ":" + QString::number(portNum, 10);
    ui->lblStatus->setText(status);
    emptyQueue();
    NetworkTimer->stop();
    OutputTimer->start( outputTime );
}


void NetworkWidget::emptyQueue()
{
    stats_discarded += queue.size();
    queue.clear();
    ui->lblQueueCurrent->setText("0");
    ui->lblDiscarded->setText(QString::number(stats_discarded,10));
}

