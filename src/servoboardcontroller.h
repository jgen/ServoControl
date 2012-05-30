#ifndef SERVOBOARDCONTROLLER_H
#define SERVOBOARDCONTROLLER_H

#include <QObject>

#include <abstractserial.h>
#include "servoboard_main.h"


class ServoboardController : public QObject
{
    Q_OBJECT
public:
    explicit ServoboardController(AbstractSerial* port, servoboard_main* form, QObject *parent = 0);

    AbstractSerial* returnSerialPort();

signals:

public slots:
    void loadFile();
    void saveFile();
    void saveFileAs();

private:
    explicit ServoboardController(QObject *parent = 0); //A kitten dies when you call this and segfault so don't
    void init();

    AbstractSerial* port;
    servoboard_main* view;


};

#endif // SERVOBOARDCONTROLLER_H
