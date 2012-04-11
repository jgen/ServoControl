#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <QtGui/QMessageBox>

#include "serial_widget.h"
#include "ui_serial_widget.h"
#include "infowidget.h"
#include "tracewidget.h"

#include <serialdeviceenumerator.h>
#include <abstractserial.h>




SerialWidget::SerialWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialWidget),
    infoWidget(0), traceWidget(0), enumerator(0), serial(0), port(0)
{
    ui->setupUi(this);

    this->initEnumerator();
    this->initSerial();
    this->initButtonConnections();
    this->initBoxConnections();
    this->initSerialWidgetCloseState();
}

SerialWidget::~SerialWidget()
{
    this->deinitEnumerator();
    this->deinitSerial();
    this->deinitWidgets();

    delete ui;
}

void SerialWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

/* Private slots section */

void SerialWidget::procEnumerate(const QStringList &l)
{
    // Fill ports box.
    ui->portBox->clear();
    ui->portBox->addItems(l);
}

void SerialWidget::procSerialMessages(const QString &msg, QDateTime dt)
{
    //qDebug() << dt.time().toString() << " > " << msg;

    // we will ignore the datetime that the signal sends as we add the datetime in the debug log viewer anyways.
    qDebug() << msg;
}

void SerialWidget::procSerialDataReceive()
{
    if (this->initTraceWidget() && this->port && this->port->isConnected()) {
        QByteArray data = this->port->readAll();
        qDebug() << "Rx: " << data;
        this->traceWidget->printTrace(data, true);
    }
}

void SerialWidget::procSerialDataTransfer(const QByteArray &data)
{
    if (this->port && this->port->isConnected())
        this->port->write(data);
}

void SerialWidget::procCtsChanged(bool val)
{
    ui->ctsLabel->setEnabled(val);
}

void SerialWidget::procDsrChanged(bool val)
{
    ui->dsrLabel->setEnabled(val);
}

void SerialWidget::procRingChanged(bool val)
{
    ui->ringLabel->setEnabled(val);
}

void SerialWidget::procControlButtonClick()
{
    if (this->serial) {
        bool result = this->port->isConnected();
        if (result) {
            this->port->close();
            result = false;
        }
        else
        {
            result = this->port->serialConnect(ui->portBox->currentText());
            this->deinitOptionsWidget();
        }

        (result) ? this->initSerialWidgetOpenState() : this->initSerialWidgetCloseState();
    }
}
void SerialWidget::deinitOptionsWidget()
{
    this->ui->baudBox->clear();
    this->ui->dataBox->clear();
    this->ui->parityBox->clear();
    this->ui->stopBox->clear();
    this->ui->flowBox->clear();
}

void SerialWidget::procInfoButtonClick()
{
    if (this->initInfoWidget()) {
        this->updateInfoData(ui->portBox->currentText());
        this->infoWidget->show();
    }
}

void SerialWidget::procIOButtonClick()
{
    if (this->initTraceWidget() && this->port && this->port->isConnected()) {
        this->traceWidget->setTitle(this->port->getDeviceName());
        this->traceWidget->show();
    }
}

void SerialWidget::procRtsButtonClick()
{
    bool result = this->port && this->port->isConnected();
    if (result) {
        // Get Rts state
        result = SerialWrapper::LineRTS & this->port->lineStatus();
        this->port->setRTS(!result);
        this->detectSerialLineStates();
    }
}

void SerialWidget::procDtrButtonClick()
{
    bool result = this->serial && this->serial->isOpen();
    if (result) {
        // Get Dtr state
        result = AbstractSerial::LineDTR & this->serial->lineStatus();
        this->serial->setDtr(!result);
        this->detectSerialLineStates();
    }
}

void SerialWidget::procBoxChange(const QString &item)
{
    if (this->initInfoWidget())
        this->updateInfoData(item);
}

void SerialWidget::procOptionsBoxChanged()
{
    if (ui->groupOptions->isEnabled())
        ui->btnApplyOptions->setEnabled(true);
}

void SerialWidget::on_btnApplyOptions_pressed()
{
    if (this->serial && this->serial->isOpen()) {
        QStringList notApplyList;
        QString setting;
        bool result = true;

        setting = this->ui->baudBox->currentText();
        if ((this->serial->baudRate() != setting) && (!this->serial->setBaudRate( setting ))) {
            // failed to apply
            notApplyList << setting;
            result = false;
        }

        setting = this->ui->dataBox->currentText();
        if ((this->serial->dataBits() != setting) && (!this->serial->setDataBits( setting ))) {
            // failed to apply
            notApplyList << setting;
            result = false;
        }

        setting = this->ui->parityBox->currentText();
        if ((this->serial->parity() != setting) && (!this->serial->setParity( setting ))) {
            // failed to apply
            notApplyList << setting;
            result = false;
        }

        setting = this->ui->stopBox->currentText();
        if ((this->serial->stopBits() != setting) && (!this->serial->setStopBits( setting ))) {
            // failed to apply
            notApplyList << setting;
            result = false;
        }

        setting = this->ui->flowBox->currentText();
        if ((this->serial->flowControl() != setting) && (!this->serial->setFlowControl( setting ))) {
            // failed to apply
            notApplyList << setting;
            result = false;
        }

        if (!result) {
            QMessageBox msgBox;
            msgBox.setText("Error.");
            QString notApplStr;
            foreach (QString s, notApplyList) {
                notApplStr.append(QString("\n %1").arg(s));
            }

            msgBox.setInformativeText(QString(tr("Not applied: %1").arg(notApplStr)));
            msgBox.exec();
        }
    }
}

/* Private methods section */

void SerialWidget::initSerialWidgetCloseState()
{
    ui->portBox->setEnabled(true);
    ui->ioButton->setEnabled(false);
    ui->rtsButton->setEnabled(false);
    ui->dtrButton->setEnabled(false);
    ui->controlButton->setText(QString(tr("Open")));
    ui->groupOptions->setDisabled(true);

    this->detectSerialLineStates();

    if (this->traceWidget && this->traceWidget->isVisible())
        this->traceWidget->hide();
}

void SerialWidget::initSerialWidgetOpenState()
{
    ui->portBox->setEnabled(false);
    ui->ioButton->setEnabled(true);
    ui->rtsButton->setEnabled(true);
    ui->dtrButton->setEnabled(true);
    ui->controlButton->setText(QString(tr("Close")));
    ui->groupOptions->setEnabled(true);

    this->initOptionsWidget();
    this->setDefaultOptions();

    this->detectSerialLineStates();
}

bool SerialWidget::initInfoWidget()
{
    if (!this->infoWidget) {
        this->infoWidget = new InfoWidget();
        if (!this->infoWidget)
            return false;
    }
    return true;
}

void SerialWidget::initOptionsWidget()
{
    // Populate the options boxes
    //TODO This could be a memory leak, check later
    this->ui->baudBox->addItems(    this->port->listBaudRate() );
    this->ui->dataBox->addItems(    this->port->listDataBits() );
    this->ui->parityBox->addItems(  this->port->listParity() );
    this->ui->stopBox->addItems(    this->port->listStopBits() );
    this->ui->flowBox->addItems(    this->port->listFlowControl() );
}

void SerialWidget::setDefaultOptions()
{
    // First select the defaults in the GUI,
    // so the user can see what is currently set.

    int result = -1;
    result = this->ui->baudBox->findText("9600 baud");
    if (result)
        this->ui->baudBox->setCurrentIndex(result);

    result = -1;
    result = this->ui->dataBox->findText("8 bit");
    if (result)
        this->ui->dataBox->setCurrentIndex(result);

    result = -1;
    result = this->ui->parityBox->findText("None");
    if (result)
        this->ui->parityBox->setCurrentIndex(result);

    result = -1;
    result = this->ui->stopBox->findText("1");
    if (result)
        this->ui->stopBox->setCurrentIndex(result);

    result = -1;
    result = this->ui->flowBox->findText("Disable");
    if (result)
        this->ui->flowBox->setCurrentIndex(result);

    // Apply the options to the serial port
    this->serial->setBaudRate(this->serial->BaudRate9600);
    this->serial->setDataBits(this->serial->DataBits8);
    this->serial->setParity(this->serial->ParityNone);
    this->serial->setStopBits(this->serial->StopBits1);
    this->serial->setFlowControl(this->serial->FlowControlOff);
}

bool SerialWidget::initTraceWidget()
{
    if (!this->traceWidget) {
        this->traceWidget = new TraceWidget();
        if (!this->traceWidget)
            return false;

        connect(this->traceWidget, SIGNAL(sendSerialData(QByteArray)),
                this, SLOT(procSerialDataTransfer(QByteArray)));
    }
    return true;
}

void SerialWidget::initEnumerator()
{
    if (!this->enumerator)
        this->enumerator = SerialDeviceEnumerator::instance();
    connect(this->enumerator, SIGNAL(hasChanged(QStringList)), this, SLOT(procEnumerate(QStringList)));
    this->procEnumerate(this->enumerator->devicesAvailable());
}

void SerialWidget::deinitEnumerator()
{
}

void SerialWidget::initSerial()
{
    if (this->serial)
        return;
    this->serial = new AbstractSerial(this);
    this->port = new SerialWrapper(this,serial);
    connect(this->port, SIGNAL(signalStatus(QString,QDateTime)), this, SLOT(procSerialMessages(QString,QDateTime)));
    connect(this->port, SIGNAL(ctsChanged(bool)), this, SLOT(procCtsChanged(bool)));
    connect(this->port, SIGNAL(dsrChanged(bool)), this, SLOT(procDsrChanged(bool)));
    connect(this->port, SIGNAL(ringChanged(bool)), this, SLOT(procRingChanged(bool)));
    connect(this->port, SIGNAL(readyRead()), this, SLOT(procSerialDataReceive()));

    // Enable emmiting signal signalStatus
    this->serial->enableEmitStatus(true);
}

void SerialWidget::deinitSerial()
{
    if (this->port && this->port->isConnected())
        this->port->close();

}

void SerialWidget::initButtonConnections()
{
    connect(ui->controlButton, SIGNAL(clicked()), this, SLOT(procControlButtonClick()));
    connect(ui->infoButton, SIGNAL(clicked()), this, SLOT(procInfoButtonClick()));
    connect(ui->ioButton, SIGNAL(clicked()), this, SLOT(procIOButtonClick()));

    connect(ui->rtsButton, SIGNAL(clicked()), this, SLOT(procRtsButtonClick()));
    connect(ui->dtrButton, SIGNAL(clicked()), this, SLOT(procDtrButtonClick()));
}

void SerialWidget::initBoxConnections()
{
    connect(ui->portBox, SIGNAL(currentIndexChanged(QString)), this, SLOT(procBoxChange(QString)));

    // For the Port Options Group
    connect(ui->baudBox, SIGNAL(currentIndexChanged(int)), this, SLOT(procOptionsBoxChanged()));
    connect(ui->dataBox, SIGNAL(currentIndexChanged(int)), this, SLOT(procOptionsBoxChanged()));
    connect(ui->parityBox, SIGNAL(currentIndexChanged(int)), this, SLOT(procOptionsBoxChanged()));
    connect(ui->stopBox, SIGNAL(currentIndexChanged(int)), this, SLOT(procOptionsBoxChanged()));
    connect(ui->flowBox, SIGNAL(currentIndexChanged(int)), this, SLOT(procOptionsBoxChanged()));
}

void SerialWidget::deinitWidgets()
{
    if (this->infoWidget)
        delete (this->infoWidget);
}

void SerialWidget::setRtsDtrButtonsCaption(bool opened, bool rts, bool dtr)
{
    if (!opened) {
        ui->rtsButton->setText(QString(tr("Control RTS")));
        ui->dtrButton->setText(QString(tr("Control DTR")));
        return;
    }
    (rts) ? ui->rtsButton->setText(QString(tr("Clear RTS"))) : ui->rtsButton->setText(QString(tr("Set RTS")));
    (dtr) ? ui->dtrButton->setText(QString(tr("Clear DTR"))) : ui->dtrButton->setText(QString(tr("Set DTR")));
}

void SerialWidget::detectSerialLineStates()
{
    bool opened = this->port && this->port->isConnected();
    quint16 line = 0;

    if (opened)
        line = this->port->lineStatus();

    this->setRtsDtrButtonsCaption(opened,
                                  SerialWrapper::LineRTS & line, SerialWrapper::LineDTR & line);

    ui->ctsLabel->setEnabled(SerialWrapper::LineCTS & line);
    ui->dcdLabel->setEnabled(SerialWrapper::LineDCD & line);
    ui->dsrLabel->setEnabled(SerialWrapper::LineDSR & line);
    ui->dtrLabel->setEnabled(SerialWrapper::LineDTR & line);
    ui->leLabel->setEnabled(SerialWrapper::LineLE & line);
    ui->ringLabel->setEnabled(SerialWrapper::LineRI & line);
    ui->rtsLabel->setEnabled(SerialWrapper::LineRTS & line);
}

void SerialWidget::updateInfoData(const QString &name)
{
    if (this->enumerator && this->infoWidget) {
        InfoWidget::InfoData data;

        this->enumerator->setDeviceName(name);
        data.name = name;
        data.bus = this->enumerator->bus();
        data.busy = this->enumerator->isBusy();
        data.description = this->enumerator->description();
        data.driver = this->enumerator->driver();
        data.exists = this->enumerator->isExists();
        data.friendlyName = this->enumerator->friendlyName();
        data.locationInfo = this->enumerator->locationInfo();
        data.manufacturer = this->enumerator->manufacturer();
        data.productID = this->enumerator->productID();
        data.revision = this->enumerator->revision();
        data.service = this->enumerator->service();
        data.shortName = this->enumerator->shortName();
        data.subSystem = this->enumerator->subSystem();
        data.systemPath = this->enumerator->systemPath();
        data.vendorID = this->enumerator->vendorID();

        this->infoWidget->updateInfo(data);
    }
}


/* This is probably a  _bad idea_  but, is used for the network_widget */
AbstractSerial* SerialWidget::getSerialPointer() {
    return serial;
}

