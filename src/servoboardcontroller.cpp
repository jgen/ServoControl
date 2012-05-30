#include "servoboardcontroller.h"

ServoboardController::ServoboardController(QObject *parent) :
    QObject(parent)
{
}
ServoboardController::ServoboardController(AbstractSerial *port,
                                           servoboard_main *form,
                                           QObject *parent):
QObject(parent),
port(port),
view(form)
{

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
