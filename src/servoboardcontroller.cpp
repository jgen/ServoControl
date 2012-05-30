#include "servoboardcontroller.h"

ServoboardController::ServoboardController(QObject *parent) :
    QObject(parent),
    port(0),
    view(0)
{
    this->init();
}
ServoboardController::ServoboardController(AbstractSerial *port,
                                           servoboard_main *form,
                                           QObject *parent):
QObject(parent),
port(port),
view(form)
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

}

/*Public Methods*/
AbstractSerial* ServoboardController::returnSerialPort()
{
    return port;
}

/*Public Slot*/
void ServoboardController::loadFile()
{

}
void ServoboardController::saveFile()
{

}
void ServoboardController::saveFileAs()
{

}
