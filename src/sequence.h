#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <QObject>
#include <QFile>
#include <QVector>
#include <QLatin1Char>
#include "position.h"


typedef  QVector<Position*> Positions;//Not sure what is best, can figure that out later.
class Sequence : public QObject
{
    Q_OBJECT
public:
    explicit Sequence(QObject *parent = 0);
    ~Sequence();
    QString toString(bool* okay = 0);
    bool fromString(QString data); //copy on write, so no need to use a reference
    bool toFile(QFile& outputFile);
    bool toFile (QString outputFileName);
    bool fromFile(QFile& inputFile);
    bool fromFile(QString inputFileName);

    void addPosition(Position* newPosition);

    //TODO: Add stream operators for string and file stream.

signals:

public slots:

private:
    void init();
    bool parseFileHeader(QString& header);
    bool ParseRangeStore(const QString& source, quint8& dest, int min, int max);
    QString toFileString(bool* ok = 0);
    bool fromFileString(QTextStream& stream);

    QString headerToString();


    Positions m_positions;
    quint8 m_sequenceReplay;//Stores the value to be sent to serial port(key)
    /* RS: Number of times a sequence is repeated. (value)
       LS: Number sent to the serveo (key)
    FFF = Sequence Replay
            0=======1
            1=======2
            2=======10
            3=======25
            4=======50
            5=======100
            6=======150
            7=======200
    */

    quint8 m_PWMRepeat;
    /*
        CCC = PWM Repeat
        RS: Number of times a signal is sent to a servo, before the sweep is added to it.
        (RS * 8ms = time servo sits still between sweep increments)
        LS: Value sent  to the board to get the RS delay.
        0=======1
        1=======2
        2=======10
        3=======25
        4=======50
        5=======100
        6=======150
        7=======200
    */

    quint8 m_PWMSweep;
    /*
    DDD = PWM Sweep - Inc/Dec value for PWM during sweep to target.(0 = no sweep)
            000 -> 015
    */

    quint8 m_sequenceDelay;
    /*
    EEE = Sequence Delay - Time between positions being sent
            000 -> 015
    */

    quint8 m_runFormat;
    /*
     AAA = Run format //Ignored, can be used to denote formatting but not needed.
             0=======Direct Run
             1=======Sequence 12
             2=======Sequence 3
             3=======Sequence 1
             4=======Freeze
   */

    quint8 m_databank;
    /*
    BBB = Data Bank //Unneeded, can be ignored
        000 -> 005
    */
    QMap<quint8,int> m_replayMap; //Index = key, Replays= value;
    QMap<int,QString> m_comments; //Line Number Comment from = key , comment = value
    bool m_hasData;

};

#endif // SEQUENCE_H
