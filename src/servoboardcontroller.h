#ifndef SERVOBOARDCONTROLLER_H
#define SERVOBOARDCONTROLLER_H

#include <QObject>
#include <QFileDialog>

#include <abstractserial.h>
#include "servoboard_main.h"
#include "sequence.h"
#include "position.h"


class ServoboardController : public QObject
{
    Q_OBJECT
public:
    explicit ServoboardController(AbstractSerial* port, servoboard_main* form, QObject *parent = 0);
    ~ServoboardController();
    AbstractSerial* returnSerialPort();

signals:

public slots:
    void loadFile();
    void saveFile();
    void saveFileAs();
    void newPositionForSequence(Position* p);

private:
    explicit ServoboardController(QObject *parent = 0); //A kitten dies when you call this and segfault so don't
    void init();

    bool checkForChangesToTextSequence();

    AbstractSerial* port;
    servoboard_main* view;
    Sequence* displayedData;



};

#endif // SERVOBOARDCONTROLLER_H
