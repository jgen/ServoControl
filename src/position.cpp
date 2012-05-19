#include "position.h"

Position::Position(QObject *parent) :
        QObject(parent),
        m_PWMRepeatMap(),
        m_boardNumber(1),
        m_data(),
        m_hasData(false),
        m_isFreeze(false),
        m_hasDelay(false),
        m_hasPWM(false)
{
    init();
}
/* Initialization code*/
void Position::init()
{
    m_PWMRepeatMap.insert(0,1);
    m_PWMRepeatMap.insert(1,2);
    m_PWMRepeatMap.insert(2,10);
    m_PWMRepeatMap.insert(3,25);
    m_PWMRepeatMap.insert(4,50);
    m_PWMRepeatMap.insert(5,100);
    m_PWMRepeatMap.insert(6,150);
    m_PWMRepeatMap.insert(7,200);

    for(quint8 i = 1; i < 16; i++)
    {
        quint8 d = 1;
        m_data.insert(i,d);
    }
    qDebug() << this->toServoSerialData();
    //qDebug() << this->fromString(this->toString());
    //qDebug() << this->toString();
}

/*Public Methods*/

//TODO: Do we want to reinitalize to default state on an invalid input,
//or leave in an undetermined state?
bool Position::fromString(QString input)
{
    if(input.startsWith("*"))
    {
        m_isFreeze = false;
    }
    else if(input.startsWith("&"))
    {
        m_isFreeze = true;
    }
    else
    {
        return false;//Doesn't start with the correct symbol
    }
    input = input.remove(0,1);
    QStringList info = input.split(QChar(','));
    if (info.length() < 2 || (info.length() % 2) != 0)
    {
        return false; //To short, or wrong format
    }
    if (!this->parseStartOfString(info))
    {
        return false; //Problem parsing PWM data
    }
    if (!this->parseServoPositions(info))
    {
        return false; //Problem parsing servo data
    }
    if (info.isEmpty())
    {
        return true; //Successful parse
    }
    if(info.at(0) != "SeqDelay")
    {
        return false; //Unknown identifier after servo positions
    }
    bool ok = false;
    quint8 seqDelay = info.at(1).toUShort(&ok,10);
    if (!ok || seqDelay < 0 || seqDelay > 15)
    {
        return false; //Sequence delay was invalid or out of range;
    }
    m_data.insert(Position::SeqDelay, seqDelay);
    return true;
}


QString Position::toString()
{
    QString output = createStartOfString();
    for(int servoNumber = 1; servoNumber <= 12; servoNumber++)
    {
        if(m_data.contains(servoNumber))
        {
            this->addTerminatingComma(output);
            output.append(QString("%1,%2").arg(servoNumber,3,10,QLatin1Char('0'))
                          .arg(m_data.value(servoNumber),3,10,QLatin1Char('0')));
        }
    }
    if(m_data.contains(Position::SeqDelay))
    {
        this->addTerminatingComma(output);
        output.append(QString("SeqDelay,%1").arg(m_data.value(SeqDelay),3,10,QLatin1Char('0')));
    }
    return output;

}

QByteArray Position::toServoSerialData()
{
    QByteArray result;
    for (quint8 servoNumber = 1; servoNumber < 13; ++servoNumber)
    {
        if (m_data.contains(servoNumber))
        {
            quint8 address =0;
            this->bitSet(address,7);
            quint8 boardNum = m_boardNumber;
            boardNum *=16;
            address |= boardNum;
            address |= servoNumber;
            result.append(address);
            result.append(m_data.value(servoNumber));
        }
    }
    return result;
}

void Position::bitClear(quint8 &byte, int position)
{
    if (position > 8 || position < 0)
    {
        return;
    }
    unsigned char mask = 0;
    switch(position)
    {
    case 0:
        mask = 0xFE;//1111 1110
        break;
    case 1:
        mask = 0xFD;//1111 1101
        break;
    case 2:
        mask = 0xFB;//1111 1011
        break;
    case 3:
        mask = 0xF7;//1111 0111
        break;
    case 4:
        mask = 0xEF;//1110 1111
        break;
    case 5:
        mask = 0xDF;//1101 1111
        break;
    case 6:
        mask = 0xBF;//1011 0000
        break;
    case 7:
        mask = 0x7F;//0111 1111
        break;
    }
    byte &= mask;
}
void Position::bitSet(quint8 &byte, int position)
{
    if (position > 8 || position < 0)
    {
        return;
    }
    unsigned char mask = 0;
    switch(position)
    {
    case 0:
        mask = 0x01;//0000 0001
        break;
    case 1:
        mask = 0x02;//0000 0010
        break;
    case 2:
        mask = 0x04;//0000 0100
        break;
    case 3:
        mask = 0x08;//0000 1000
        break;
    case 4:
        mask = 0x10;//0001 0000
        break;
    case 5:
        mask = 0x20;//0010 0000
        break;
    case 6:
        mask = 0x40;//0100 0000
        break;
    case 7:
        mask = 0x80;//1000 0000
        break;
    }
    byte |= mask;
}
int Position::getBoardNumber()
{
    return this->m_boardNumber;
}

bool Position::setBoardNumber(int boardNumber)
{
    if (boardNumber > 7 || boardNumber < 1)
    {
        return false;
    }
    this->m_boardNumber = boardNumber;
    return true;
}

/*Private Methods*/

QString Position::createStartOfString()
{
    QString output = "";
    if (m_isFreeze)
    {
        output.append("&");
    }
    else
    {
        output.append("*");
    }
    if (m_data.contains(PWMRepeat))
    {
        output.append(QString("PWMRep,%1")
                      .arg(m_PWMRepeatMap.value(m_data.value(PWMRepeat)),3,10,QLatin1Char('0')));
    }
    if (m_data.contains(PWMSweep))
    {
        this->addTerminatingComma(output);
        output.append(QString("PWMSweep,%1")
                      .arg(m_data.value(PWMSweep),3,10,QLatin1Char('0')));
    }
    return output;
}

void Position::addTerminatingComma(QString& string)
{
    if (!string.endsWith(",") || !string.endsWith("*") || !string.endsWith("&"))
    {
        string.append(",");
    }
}
bool Position::parseServoPositions(QStringList &input)
{
    //Each line must have at least one servo position.
    if (input.isEmpty())
    {
        return false; //No servo positions
    }
    bool hasPositions = false;
    while(!input.isEmpty() && input.at(0) != "SeqDelay")
    {
        bool ok = false;
        quint8 servoNum = input.at(0).toUShort(&ok,10);
        if (!ok || servoNum < 0 || servoNum > 12)
        {
            return false; //Servo number not valid/outo fo range
        }
        ok = false;
        quint8 servoData = input.at(1).toUShort(&ok,10);
        if (!ok || servoData < 1 || servoData > 97)
        {
            return false;//Servo data not valid/out of range
        }
        m_data.insert(servoNum,servoData);
        hasPositions = true;
        input.removeFirst();
        input.removeFirst();
    }
    if (hasPositions)
    {
        return true;
    }
    else
    {
        return false; //No servo postions
    }

}

bool Position::parseStartOfString(QStringList& info)
{

    if (info.at(0) == "PWMRep")
    {
        bool ok = false;
        quint8 data = info.at(1).toUShort(&ok,10);
        if (!ok|| data < 0 || data > 7)
        {
            return false; //PWM Repeat value wrong.
        }
        data = m_PWMRepeatMap.key(data);
        this->m_data.insert(Position::PWMRepeat,data);
        info.removeFirst();
        info.removeFirst();

        if (info.at(0) == "PWMSweep") //If there is PWMRep, there must be sweep as well.
        {
            bool ok = false;
            quint8 data = info.at(1).toUShort(&ok,10);
            if (!ok || data > 15 || data < 0)
            {
                return false; //PWM sweep values wrong
            }
            this->m_data.insert(Position::PWMSweep,data);
            info.removeFirst();
            info.removeFirst();
        }
        this->m_hasPWM = true;
        return true;
    }
    this->m_hasPWM = false;
    return true;


}


