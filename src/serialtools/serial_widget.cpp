#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <QtGui/QMessageBox>

#include "serial_widget.h"
#include "ui_serial_widget.h"
#include "infowidget.h"
#include "tracewidget.h"

#include <serialdeviceenumerator.h>
#include <abstractserial.h>

#include "connectioncontroller.h"


SerialWidget::SerialWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SerialWidget),
    infoWidget(0), traceWidget(0), enumerator(0)
{
    ui->setupUi(this);


    this->initButtonConnections();
    this->initBoxConnections();
    this->initSerialWidgetCloseState();
}

SerialWidget::~SerialWidget()
{
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

//duplicate of procEnumerate slot, this will be the version of the
//method that is prefered to be used.
void SerialWidget::updateEnumeratedDevices(const QStringList &l)
{
    ui->portBox->clear();

    if (l.size() <= 0) {
        // No devices are available.
        ui->infoButton->setDisabled(true);
        ui->controlButton->setDisabled(true);
        QString status = "No Serial Ports Found!";
        ui->lblPortsFound->setText(status);
    } else {
        // Fill ports box.
        ui->portBox->addItems(l);
        ui->infoButton->setDisabled(false);
        ui->controlButton->setDisabled(false);

        QString status = QString::number(l.size()) + " Serial Port(s) Found!";

        ui->lblPortsFound->setText(status);
    }
}


void SerialWidget::procSerialMessages(const QString &msg, QDateTime dt)
{
    // we will ignore the datetime that the signal sends as we add the datetime in the debug log viewer anyways.
    qDebug() << msg;
}


//Public access tot printing onto trace widget
void SerialWidget::printToTrace(QByteArray data, bool isRx)
{
    if (this->initTraceWidget())
    {
       this->traceWidget->printTrace(data,isRx);
    }
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
    //Connecting and disconnecting button.
    if (this->ui->controlButton->text() == "Open")
    {
        control->open(ui->portBox->currentText());
    }
    else
    {
        control->close();
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
        // update the info widget with the port information
        control->updateInfoWidget(ui->portBox->currentText());
        //this->updateInfoData(ui->portBox->currentText());
        this->infoWidget->show(); // show it
    }
}

void SerialWidget::procIOButtonClick()
{
    if (this->initTraceWidget()) {
        this->traceWidget->setTitle(this->ui->portBox->currentText());
        this->traceWidget->show();
    }
}

void SerialWidget::procRtsButtonClick()
{
    control->RTSToggle();
}

void SerialWidget::procDtrButtonClick()
{
    control->DTRToggle();
}

void SerialWidget::procBoxChange(const QString &item)
{
    if (this->initInfoWidget());
}

void SerialWidget::procOptionsBoxChanged()
{
    if (ui->groupOptions->isEnabled())
        ui->btnApplyOptions->setEnabled(true);
}

void SerialWidget::on_btnApplyOptions_pressed()
{
    control->changeConnectionParameters(this->ui->baudBox->currentText(),
                                        this->ui->dataBox->currentText(),
                                        this->ui->parityBox->currentText(),
                                        this->ui->stopBox->currentText(),
                                        this->ui->flowBox->currentText());
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


    if (this->traceWidget && this->traceWidget->isVisible())
        this->traceWidget->hide();
}
void SerialWidget::initSerialWidgetCloseState(quint16 line)
{
    ui->portBox->setEnabled(true);
    ui->ioButton->setEnabled(false);
    ui->rtsButton->setEnabled(false);
    ui->dtrButton->setEnabled(false);
    ui->controlButton->setText(QString(tr("Open")));
    ui->groupOptions->setDisabled(true);

    this->updateSerialLineStates(line,false);

    if (this->traceWidget && this->traceWidget->isVisible())
        this->traceWidget->hide();
}
void SerialWidget::initSerialWidgetOpenState(quint16 line)
{
    ui->portBox->setEnabled(false);
    ui->ioButton->setEnabled(true);
    ui->rtsButton->setEnabled(true);
    ui->dtrButton->setEnabled(true);
    ui->controlButton->setText(QString(tr("Close")));
    ui->groupOptions->setEnabled(true);

   // this->initOptionsWidget();
    //this->setDefaultOptions();

    this->updateSerialLineStates(line, true);
}

void SerialWidget::initSerialWidgetOpenState()
{
    ui->portBox->setEnabled(false);
    ui->ioButton->setEnabled(true);
    ui->rtsButton->setEnabled(true);
    ui->dtrButton->setEnabled(true);
    ui->controlButton->setText(QString(tr("Close")));
    ui->groupOptions->setEnabled(true);

    this->setDefaultOptions();

}

bool SerialWidget::initInfoWidget()
{
    // Check if we already have an info widget.
    if (!this->infoWidget) {
        // if not, make one
        this->infoWidget = new InfoWidget();
        if (!this->infoWidget)
            return false;
    }
    return true;
}

void SerialWidget::updateOptionsWidget(QStringList& baudrates, QStringList& dataBits,
                                       QStringList& parity, QStringList& stopBits,
                                       QStringList& flow)
{
    this->ui->baudBox->clear();
    this->ui->baudBox->addItems(baudrates);
    this->ui->dataBox->clear();
    this->ui->dataBox->addItems(dataBits);
    this->ui->parityBox->clear();
    this->ui->parityBox->addItems(parity);
    this->ui->stopBox->clear();
    this->ui->stopBox->addItems(stopBits);
    this->ui->flowBox->clear();
    this->ui->flowBox->addItems(flow);
    this->setDefaultOptions();
}
void SerialWidget::setDefaultOptions()
{
    // First select the defaults in the GUI,
    // so the user can see what is currently set.
    //These will be set in a number of places,
    //should be factored to one when there is time.

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
}

bool SerialWidget::initTraceWidget()
{
    // check if we already have a Trace widget
    if (!this->traceWidget) {
        // if not, make one
        this->traceWidget = new TraceWidget();
        if (!this->traceWidget)
            return false;


    }
    connect(this->traceWidget, SIGNAL(sendSerialData(QByteArray)),
            control,SLOT(sendSerialData(QByteArray)));
    return true;
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
void SerialWidget::updateSerialLineStates(quint16 line,bool connected)
{

    this->setRtsDtrButtonsCaption(connected,
                                  AbstractSerial::LineRTS & line, AbstractSerial::LineDTR & line);

    ui->ctsLabel->setEnabled(AbstractSerial::LineCTS & line);
    ui->dcdLabel->setEnabled(AbstractSerial::LineDCD & line);
    ui->dsrLabel->setEnabled(AbstractSerial::LineDSR & line);
    ui->dtrLabel->setEnabled(AbstractSerial::LineDTR & line);
    ui->leLabel->setEnabled(AbstractSerial::LineLE & line);
    ui->ringLabel->setEnabled(AbstractSerial::LineRI & line);
    ui->rtsLabel->setEnabled(AbstractSerial::LineRTS & line);
}

void SerialWidget::updateInfoData(SerialDeviceEnumerator* enumerator, QString& name)
{
    //Parameters: emunerator to all serial ports, name of the port info to be diplayed about.
    if (enumerator && infoWidget) {
        InfoWidget::InfoData data;

        enumerator->setDeviceName(name);
        data.name = name;
        data.bus = enumerator->bus();
        data.busy = enumerator->isBusy();
        data.description = enumerator->description();
        data.driver = enumerator->driver();
        data.exists = enumerator->isExists();
        data.friendlyName = enumerator->friendlyName();
        data.locationInfo = enumerator->locationInfo();
        data.manufacturer = enumerator->manufacturer();
        data.productID = enumerator->productID();
        data.revision = enumerator->revision();
        data.service = enumerator->service();
        data.shortName = enumerator->shortName();
        data.subSystem = enumerator->subSystem();
        data.systemPath = enumerator->systemPath();
        data.vendorID = enumerator->vendorID();

        this->infoWidget->updateInfo(data);
    }
}
/*

    Returns the AbstractSerial Object.
    This allows direct access to the Serial object.

    This is probably a  *bad idea*  but without making it a program wide
    global, I don't know how to share it with all the other widgets...

*/
/*AbstractSerial* SerialWidget::getSerialPointer() {
    return serial;
}*/
void SerialWidget::setConnectionController(ConnectionController *con)
{
    this->control = con;
    connect(this->control,SIGNAL(CTSChanged(bool)),SLOT(procCtsChanged(bool)));
    connect(this->control,SIGNAL(DSRChanged(bool)),SLOT(procDsrChanged(bool)));
    connect(this->control,SIGNAL(RingChanged(bool)),SLOT(procRingChanged(bool)));
}
void SerialWidget::clearConnectionController()
{
    this->control = 0;
}

