#include "position.h"


Position::Position() :
        m_PWMRepeatMap(),
        m_data(),
        m_boardNumber(1),
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
    else if (input.isEmpty())
    {
        this->m_hasData = false;
        return true;
    }
    else
    {
        qDebug () << input;
        qDebug() << "Error parsing in line starting character position";
        return false;//Doesn't start with the correct symbol
    }
    input = input.remove(0,1);
    QStringList info = input.split(QChar(','));
    if (info.length() < 2 || (info.length() % 2) != 0)
    {
        qDebug() << "Error parsing line length and format in position";
        return false; //To short, or wrong format
    }
    if (!this->parseStartOfString(info))
    {
        qDebug() << "Error parsing PWM data in position";
        return false; //Problem parsing PWM data
    }
    if (!this->parseServoPositions(info))
    {
        qDebug() << "Error parsing servo data in position";
        return false; //Problem parsing servo data
    }
    if (info.isEmpty())
    {
        return true; //Successful parse
    }
    if(info.at(0) != "SeqDelay")
    {
        qDebug() << "Error parsing unknown identifier after servo positions in position";
        return false; //Unknown identifier after servo positions
    }
    bool ok = false;
    quint8 seqDelay = info.at(1).toUShort(&ok,10);
    if (!ok || seqDelay < 0 || seqDelay > 15)
    {
        qDebug() << "Error parsing sequence delay invalid or out of range in position";
        return false; //Sequence delay was invalid or out of range;
    }
    this->m_hasDelay = true;
    m_data.insert(Position::SeqDelay, seqDelay);
    return true;
}


QString Position::toString(bool legacyMode)
{
    QString output("");
    if (legacyMode)
    {
        output.append("*");
    }
    else
    {
        output.append(createStartOfString());
    }
    for(int servoNumber = 1; servoNumber <= 12; servoNumber++)
    {
        if(m_data.contains(servoNumber))
        {
            this->addTerminatingComma(output);
            output.append(QString("%1,%2").arg(servoNumber,3,10,QLatin1Char('0'))
                          .arg(m_data.value(servoNumber),3,10,QLatin1Char('0')));
        }
    }
    if(this->m_hasDelay && m_data.contains(Position::SeqDelay))
    {
        this->addTerminatingComma(output);
        output.append(QString("SeqDelay,%1").arg(m_data.value(SeqDelay),3,10,QLatin1Char('0')));
    }
    return output;

}

QByteArray Position::toServoSerialData()
{
    //Format of Serial Data - 1AAA BBBB 0CCC CCCC
    //where AAA is the board number
    //and BBBB is the servo number
    //and CCC CCCC is the data for the position the servo is to move to.
    QByteArray result;
    if (this->m_isFreeze)
    {
        result.append((char)159); //Special command address
        result.append((char)15);//Command for freeze

    }
    for (quint8 servoNumber = 1; servoNumber < 13; ++servoNumber)
    {
        if (m_data.contains(servoNumber))
        {
            quint8 address =0;
            this->bitSet(address,7);
            quint8 boardNum = m_boardNumber;
            boardNum *=16;//Bit shifts number 4 places left
            //boardNum = boardNum << 4; should be the same as above, not sure what is more readable.
            address |= boardNum;
            address |= (servoNumber - 1);//Occupies the low nibble.
            result.append(address);
            result.append(m_data.value(servoNumber));
        }
    }
    if (this->m_isFreeze)
    {
        result.append((char)159);//Special command address
        result.append((char)0);//command for freeze
    }
    //qDebug() << result.toHex();
    return result;
}
QByteArray Position::getPWMSerialData(bool* okay)
{
    if (!this->m_hasPWM)
    {
        if (okay)
        {
            *okay = false;
        }
        qDebug() << "Error creating PWM serial data: data doesn't exist.";
        return QByteArray();
    }
    quint8 address = 158;//Number from Eugen, hardcoded in micro
    quint8 data = 0;
    quint8 repeat = m_data.value(Position::PWMRepeat);
    //qDebug() << "Repeat key: " << repeat;
    repeat *= 16; //Shift 4 places left.
    //repeat = repeat << 4; should be the same as above, not sure what is more readable.
    quint8 sweep = m_data.value(Position::PWMSweep);
    //qDebug() << "Repeat shift:" << repeat;
    //qDebug() << "Sweep: " << sweep;
    data = sweep | repeat;
    QByteArray result;
    result.append(address);
    result.append(data);

    //qDebug() << result.toHex();
    return result;


}
bool Position::hasPWMData()
{
    return this->m_hasPWM;
}

bool Position::addAdvancedPosition(SpecialFunction function, quint8 value)
{
    if (function == Position::PWMRepeat)
    {
        /*if (!this->m_PWMRepeatMap.key(value,0))
        {
            qDebug() << "Invalid PWN repeat value passed in";
            return false;
        }*/
        m_data.insert(Position::PWMRepeat,m_PWMRepeatMap.key(value));
        this->m_hasPWM = true;
    }
    else if(function == Position::PWMSweep)
    {
        if (value < 0 || value > 15)
        {
            qDebug() << "The sweep value was invalid";
            return false;
        }
        this->m_data.insert(function,value);
        this->m_hasPWM = true;
    }
    else if (function == Position::SeqDelay)
    {
        if (value < 0 || value > 15)
        {
            qDebug() << "The delay value was invalid";
            return false;
        }
        this->m_data.insert(function,value);
        this->m_hasDelay = true;
    }
    else
    {
        qDebug() << "Invalid special functions";
        return false;
    }
    return true;
}
bool Position::addAdvancedPositionIndex(SpecialFunction function, quint8 index)
{
    if (function != Position::PWMRepeat)
    {
        return this->addAdvancedPosition(function,index);
    }
    if (!this->m_PWMRepeatMap.contains(index))
    {
        qDebug() << "There was an invaid index to a PWMRepeat passed in";
        return false;
    }
    return this->addAdvancedPosition(function,this->m_PWMRepeatMap.value(index));
}
void Position::setFreeze(bool newFreeze)
{
    this->m_isFreeze = newFreeze;
}

int Position::getBoardNumber()
{
    return this->m_boardNumber;
}

bool Position::setBoardNumber(int boardNumber)
{
    if (boardNumber > 7 || boardNumber < 1)//must fit in 3 bits.
    {
        return false;
    }
    this->m_boardNumber = boardNumber;
    return true;
}
bool Position::isEmpty()
{
    if (this->m_data.size() == 0)
    {
        return true;
    }
    return false;
}

int Position::getDelay()
{
    if (this->m_hasDelay && this->m_data.contains(Position::SeqDelay))
    {
        return this->m_data.value(Position::SeqDelay);
    }
    else
    {
        return 0;
    }
}

bool Position::addServoPosition(quint8 servoNum, quint8 servoPosition)
{
    bool t;
    return this->addServoPosition(servoNum,servoPosition,t);
}

bool Position::addServoPosition(quint8 servoNum, quint8 servoPosition, bool &overwrite)
{
    overwrite = false;
    if (servoNum > 12 || servoNum < 1)
    {
        qDebug() <<  "Error adding data to position: servo number out of range";
        return false;
    }
    if (servoPosition > 97 || servoPosition < 1)
    {
        qDebug() <<  "Error adding data to position: servo position out of range";
        return false;
    }
    if (this->m_data.contains(servoNum))
    {
        overwrite = true;
    }
    m_data.insert(servoNum,servoPosition);
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
    if (m_hasPWM && m_data.contains(PWMRepeat))
    {
        output.append(QString("PWMRep,%1")
                      .arg(m_PWMRepeatMap.value(m_data.value(PWMRepeat)),3,10,QLatin1Char('0')));
    }
    if (m_hasPWM && m_data.contains(PWMSweep))
    {
        this->addTerminatingComma(output);
        output.append(QString("PWMSweep,%1")
                      .arg(m_data.value(PWMSweep),3,10,QLatin1Char('0')));
    }
    return output;
}

void Position::addTerminatingComma(QString& string)
{
    if (!string.endsWith(',') && !string.endsWith('*') && !string.endsWith('&'))
    {
        string.append(",");
    }
}
bool Position::parseServoPositions(QStringList &input)
{
    //Each line must have at least one servo position.
    if (input.isEmpty())
    {
        qDebug() << "Error parsing input string: does not exist in position";
        return false; //No servo positions
    }
    bool hasPositions = false;
    while(!input.isEmpty() && input.at(0) != "SeqDelay")
    {
        bool ok = false;
        quint8 servoNum = input.at(0).toUShort(&ok,10);
        if (!ok || servoNum < 0 || servoNum > 12)
        {
            qDebug()<< "Error parsing servo number: value out of range or wrong format in position";
            return false; //Servo number not valid/outo fo range
        }
        ok = false;
        quint8 servoData = input.at(1).toUShort(&ok,10);
        if (!ok || servoData < 1 || servoData > 97)
        {
            qDebug() << "Error parsing servo data: value out of range or wrong format in position";
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
        qDebug() << "Error parsing no servo data found in input string in position";
        return false; //No servo postions
    }

}

bool Position::parseStartOfString(QStringList& info)
{

    if (info.at(0) == "PWMRep")
    {
        bool ok = false;
        quint8 data = info.at(1).toUShort(&ok,10);
        if (!ok|| this->m_PWMRepeatMap.key(data,-1) == -1)
        {
            qDebug() << "Error parsing PWM repeat: value out of range or wrong format in position";
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
                qDebug() << "Error parsing PWM sweep: value out of range or wrong format in position";
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


