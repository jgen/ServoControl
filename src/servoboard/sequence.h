#ifndef SEQUENCE_H
#define SEQUENCE_H
/*
 *This holds a sequence of positions that can be sent to the board, including
 *the options to about how the data is sent to the board. As well it deals with
 *the file IO for reading and writing to saved sequences to memory.
 *
 *The sequence can be initalized from a file or string if needed, but is able to
 *be used without calling an explicit initalization function.
 *
 *The sequence will be reinitalized on reading from a file or a string. This will
 *loose all data that is currently stored in it. If invalid data is passed in then
 *there are no guarentees as to the state of the object afterwards, and it will
 *most likely be in an invalid internal state.
 *
 *There are two types of strings that can be output or read, the user visible strings
 *are the ones that follow the rules outlined in the sequence format and are easy to edit
 *by hand and understand. The file format strings are not written in a manner that would
 *allow for reading easily by users. Ensure that you are using the right call, as file
 *oriented commands cannot read user visible strings and vise-versa.
 *
 *There is a built in interator for moving through a sequence when it is being
 *played back over a serial connection. This must be reset before use, and cannot
 *be trusted as thread safe.
 *
 *Global sequence values can be set and changed, it will make the intelligent choice
 *as to what the next delay and repeat values are based on the order of presidence and
 *what had been currently set.
 *
 *Legacy Options: For compatibility with earlier version of the program, there is
 *a legacy flag that can be set in some functions. This is used to read and write
 *in the format from the orginal version of the program that was written by Eugen.
 *
 */

#include <QObject>
#include <QFile>
#include <QVector>
#include <QLatin1Char>
#include "position.h"




typedef  QVector<Position*> Positions;//Not sure what container is best, can figure that out later.
class Sequence : public QObject
{
    Q_OBJECT
public:
    explicit Sequence(QObject *parent = 0);
    ~Sequence();
    //Returns the sequence as a user visible string, not a file string.
    QString toString(bool* okay = 0);
    //Checks if the given string is a valid sequence.
    bool isVaild(QString data);
    //Reinitalizes based on data from a user visible string if data is valid.
    bool fromString(QString data); //copy on write, so no need to use a reference
    //This will write to a file, returning true if successful.
    bool toFile(QFile& outputFile);
    bool toFile (QString outputFileName);
    //Reinitalizes based on the file
    bool fromFile(QFile& inputFile);
    bool fromFile(QString inputFileName);

    //Adds an new position to the end of the current sequence.
    void addPosition(Position* newPosition);
    //Set the current global playback modifiers.
    bool setDelay(quint8 delay);
    bool setReplay(quint8 replay);
    bool setPWMValues(quint8 repeat, quint8 sweep);
    //Get the number of times the sequence should be replayed.
    int getRepeats(); //zero is an error

    //Resets the internal iterator to the front of the sequuence.
    void resetIterator();
    //Checks if there is another position to send in the sequence.
    bool hasNext();
    //Returns the delay between the next position to be sent and the one after it.
    int getNextDelay();
    //Returns the data to be sent for the next position (including freeze data) and
    //moves the iterator on place forward.
    QByteArray getNextData(Position *&p);

    //TODO: Add stream operators for string and file stream.

signals:

public slots:

private:
    void init();
    bool parseFileHeader(QString& header);//Reads the header from a file
    //Convience method, parses an int from the string, makes sure it is in the
    //range given and stores it. Returns true on success, false otherwise.
    bool ParseRangeStore(const QString& source, quint8& dest, int min, int max);
    //Builds the string that will be written to a file.
    QString toFileString(bool* ok = 0, bool legacyMode = false);
    //Reads a string from a file.
    bool fromFileString(QTextStream& stream);
    //Creates a string, the flag chooses whether to include PWM line data
    QString toString(bool *okay, bool legacyFormat);
    //Builds the header for a file.
    QString headerToString();

    //Holds the positions in the sequence.
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
    //used for mapping between user visible values and serial values.
    QMap<quint8,int> m_replayMap; //Index = key, Replays= value;
    //Holds the comment lines from a file, they are stored seperately from the
    //positions and readded when the output string are formed.
    QMap<int,QString> m_comments; //Line Number Comment from = key , comment = value
    //Tracks if the sequence currently has data.
    bool m_hasData;
    //The iterator for walking through a sequence.
    int m_iterator;

};

#endif // SEQUENCE_H
