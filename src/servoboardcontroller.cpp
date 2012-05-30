#include "servoboardcontroller.h"

ServoboardController::ServoboardController(QObject *parent) :
    QObject(parent),
    port(0),
    view(0),
    displayedData(0)
{
    this->init();
}
ServoboardController::ServoboardController(AbstractSerial *port,
                                           servoboard_main *form,
                                           QObject *parent):
QObject(parent),
port(port),
view(form),
displayedData(0)
{
    this->init();
}
void ServoboardController::init()
{
    if (!view)
    {
        qDebug() << tr("There was no formed passed to ServoboardController");
        //I am going to fail fast, this should never happen. And nothing
        //get attention like a segfault.
        view->disableButtons();
    }
    if (!port || !port->isOpen())//shouldn't evaluate second if first is true.
    {
        view->disableButtons();
        qDebug() << tr("No port was passed in");
        view->displayConnectionWarning();
    }
    else
    {
        port->atEnd();//No reason to call this, if there is an invalid port passsed in
        //I want to fail fast.
        view->enableButtons();
    }

    if(this->view->hasSequenceInText()) //If there is data when a tab switches, this way it can be recovered.
    {
        displayedData = new Sequence(this);

        if (!displayedData->fromString(view->currentSequenceText()))
        {
            qDebug() << "Data in the text box is not a valid sequence";
        }
        else
        {
            bool ok = false;
            view->displayNewSequence(displayedData->toString(&ok));
            if (!ok)
            {
                qDebug() << "Failed converting data to a string";
                delete displayedData;
                displayedData = 0;
                view->displayNewSequence("");
            }
        }
    }

}

ServoboardController::~ServoboardController()
{
    if(displayedData)
        delete displayedData;
}

/*Public Methods*/
AbstractSerial* ServoboardController::returnSerialPort()
{
    return port;
}

/*Public Slot*/
void ServoboardController::loadFile()
{
    QString fileName = QFileDialog::getOpenFileName(view,
                                  tr("Open Sequence"), "./", tr("Servo Sequence Files (*.SER *.SERVO)"));
    displayedData = new Sequence(this);
    if (!displayedData->fromFile(fileName))
    {
        qDebug() << tr("The file %1 is invalid").arg(fileName);
    }
    bool ok = false;
    view->displayNewSequence(displayedData->toString(&ok));
    if (!ok)
    {
        qDebug() << tr("Failed to convert the sequence to a string");
    }

}
void ServoboardController::saveFile()
{
    this->saveFileAs(); //need to properly write later

}
void ServoboardController::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(view,tr("Save Sequence As"),"./",
                                                    tr("Servo Sequnce file *.SERVO;;Eugen Servo File *.SER"));
    if (fileName.endsWith(".SER") && !view->displaySaveFormatWaring())
    {
        qDebug() << tr("Saving aborted");
    }
    displayedData->toFile(fileName);
}
void ServoboardController::newPositionForSequence(Position* p)
{
    this->displayedData->addPosition(p);
    bool ok = false;
    view->displayNewSequence(displayedData->toString(&ok));
    if (!ok)
    {
        qDebug() << tr("Error displaying sequence");
    }
}
