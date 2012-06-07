#ifndef POSITION_H
#define POSITION_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QMap>
#include <QDebug>

/*
 Storage Notes: Store PWMRepeat and SequenceReplay as 0-->15, use lookup for actual value.
*/
class Position
{
public:
    //Used as keys for the QMap to get the values for each part.
    enum SpecialFunction {PWMRepeat = 13, PWMSweep = 14, SeqDelay = 15};

    Position();


    QString toString();//Human readable string
    bool fromString(QString input);    
    //The byte array is NULL terminated, can't just get a pointer to it
    QByteArray toServoSerialData();
    QByteArray getPWMSerialData(bool* okay = 0);
    int getBoardNumber();
    bool setBoardNumber(int boardNumber);
    int getDelay();//returns zero if there is none;
    bool isEmpty();
    bool hasPWMData();

    void setFreeze(bool newFreeze);
    bool addServoPosition(quint8 servoNum, quint8 servoPosition, bool& overwrite);//False on invalid data
    bool addServoPosition(quint8 servoNum, quint8 servoPosition);
    bool addAdvancedPosition(SpecialFunction function, quint8 value);//No lookup for PWM repeat, use value as given
    bool addAdvancedPositionIndex(SpecialFunction function, quint8 index); //Will lookup for PWM repeat
signals:

public slots:

private:
    void init();
    void addTerminatingComma(QString& string);
    QString createStartOfString();
    bool parseStartOfString(QStringList& input);
    bool parseServoPositions(QStringList& input);
    void bitSet(quint8& byte, int position);
    void bitClear(quint8& byte, int position);

    QMap<quint8,int> m_PWMRepeatMap;
    QMap<quint8,quint8> m_data;
    int m_boardNumber;
    bool m_hasData;
    bool m_isFreeze;
    bool m_hasDelay;
    bool m_hasPWM;

};

#endif // POSITION_H
