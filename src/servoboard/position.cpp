#include "position.h"

/*Default constructor with useful values set*/
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

/*This initalizes the repeat map the values that are held in the
 * the mirco.*/
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

/*Reads and stores the data from a string into the object.
 * will return false and print a log message if there is an
 * error, unless it is passed an emtpy string, in which case
 * it will return true, but m_hasData wil be false*/
bool Position::fromString(QString input)
{
    //Read if there is a freeze on the line or not
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
        qDebug() << "Error: parsing in line starting character position: " <<
                    input.at(1) << " in " << "Position::fromString(QString input)" <<
                    " line: " << __LINE__;
        return false;//Doesn't start with the correct symbol
    }
    input = input.remove(0,1); //Strip the start characters
    QStringList info = input.split(QChar(','));//Split CSVs
    if (info.length() < 2 || (info.length() % 2) != 0)
    {
        qDebug() << "Error: Parsing line length and format. Number of items: "
                 << info.length() << " in " << "Position::fromString(QString input)" <<
                    " line: " << __LINE__;
        return false;
    }
    if (!this->parseStartOfString(info))//Parses and returns shortened string
    {
        qDebug() << "Error: parsing PWM data in position";
        return false;
    }
    if (!this->parseServoPositions(info))//Parse and store the servo positions
    {
        qDebug() << "Error: parsing servo data in position";
        return false;
    }
    if (info.isEmpty())//See if the sequence delay section is left in the string.
    {
        return true; //Successful parse
    }
    if(info.at(0) != "SeqDelay")
    {
        qDebug() << "Error: Unknown identifier after servo positions. Expected sequence delay"
                    << " in " << "Position::fromString(QString input)" << " line: " << __LINE__;
        return false;
    }
    bool ok = false;
    quint8 seqDelay = info.at(1).toUShort(&ok,10);
    if (!ok || seqDelay < 0 || seqDelay > 15)
    {
        qDebug() << "Error: parsing sequence delay invalid or out of range"
                    << " in " << "Position::fromString(QString input)" << " line: " << __LINE__;
        return false;
    }
    this->m_hasDelay = true;
    m_data.insert(Position::SeqDelay, seqDelay);
    return true;
}

/*
 *This writes the position to string that can be understood by the
 *user and read by fromString. The legacy mode flag is used for writing
 *to files that can be used by the older version of the program.
 *
 *This should be the inverse of fromString assuming the legacy mode
 *flag is set to false.
 *
 */
QString Position::toString(bool legacyMode)
{
    QString output("");
    if (legacyMode)//The older verion didn't use the PWM line data or freeze
    {
        output.append("*");
    }
    else
    {
        output.append(createStartOfString());//Add freeze and PWM line data to the string
    }
    for(int servoNumber = 1; servoNumber <= 12; servoNumber++) //Move through all the posible servos
    {
        if(m_data.contains(servoNumber))
        {
            this->addTerminatingComma(output); //See if a comma is needed.
            //There must always be three characters per value, even if it is only a
            //single digit. This is for ease of reading by users.
            output.append(QString("%1,%2").arg(servoNumber,3,10,QLatin1Char('0'))
                          .arg(m_data.value(servoNumber),3,10,QLatin1Char('0')));
        }
    }
    if(this->m_hasDelay && m_data.contains(Position::SeqDelay)) //Add the sequence delay
    {
        this->addTerminatingComma(output);
        output.append(QString("SeqDelay,%1").arg(m_data.value(SeqDelay),3,10,QLatin1Char('0')));
    }
    return output;

}
/*
 *This returns the data in the position object as a series of bytes encoded
 *to be sent to the board.
 *
 *Format of Serial Data - 1AAA BBBB 0CCC CCCC
 *where AAA is the board number
 *and BBBB is the servo number
 *and CCC CCCC is the data for the position the servo is to move to.
 *
 *If there is freeze data then the freeze command is predended to the return
 *value and the resume motion value is appended to it.
 */
QByteArray Position::toServoSerialData()
{
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
            //boardNum *=16;//Bit shifts number 4 places left
            boardNum = boardNum << 4; //should be the same as above, not sure what is more readable.
            address |= boardNum;
            address |= (servoNumber - 1);//Occupies the low nibble.
            result.append(address);
            result.append(m_data.value(servoNumber));
        }
    }
    if (this->m_isFreeze)
    {
        result.append((char)159);//Special command address
        result.append((char)0);//command for unfreeze
    }
    return result;
}
/*
 *This returns the two byte command to be send if there is data to be sent.
 *If there is no data to be sent, okay is made false and an empty array is returned.
 *
 *The two bytes returned are:
 *Address: 158
 *Data: 0AAA BBBB where AAA is the sweep key (index) and BBBB is the repeat
 */
QByteArray Position::getPWMSerialData(bool* okay)
{
    if (!this->m_hasPWM || !this->m_data.contains(Position::PWMRepeat) ||
           !this->m_data.contains(Position::PWMSweep) )
    {
        if (okay)
        {
            *okay = false;
        }
        qDebug() << "Error: Creating PWM serial data: data doesn't exist."
                 << " in " << "Position::getPWMSerialData(bool* okay)" << " line: " << __LINE__;
        return QByteArray();
    }
    quint8 address = 158;//Number from Eugen, hardcoded in micro
    quint8 data = 0;
    quint8 repeat = m_data.value(Position::PWMRepeat);
    //The most significant bit will be low after the shift since repeat < 8
    repeat = repeat << 4;//Shift repeat 4 places right.
    quint8 sweep = m_data.value(Position::PWMSweep);
    data = sweep | repeat; //Form the data byte
    QByteArray result;
    result.append(address);
    result.append(data);
    if (okay)
    {
        *okay = true;
    }
    return result;
}
/*
 *This returns true if there is PWM data that can be sent out to a postion,
 *otherwise it returns false.
 */
bool Position::hasPWMData()
{
    return this->m_hasPWM && this->m_data.contains(Position::PWMRepeat)
            && this->m_data.contains(Position::PWMSweep);
}
/*
 *This inserts a special function value into the position. It returns false
 *if the value passed to it is not valid, and returns true otherwise.
 *
 *It accepts the user visible value for PWM repeat,not the index value.
 *If you wish to use the index value, there is another method for that.
 */
bool Position::addAdvancedPosition(SpecialFunction function, quint8 value)
{
    if (function == Position::PWMRepeat)
    {
        //TODO: Test this and add it back in.
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
        if (value < 0 || value > 15) //This must be true.
        {
            qDebug() << "The sweep value was invalid"
                     << " in " <<
                     "Position::addAdvancedPosition(SpecialFunction function, quint8 value)"
                     << " line: " << __LINE__;
            return false;
        }
        this->m_data.insert(function,value);
        this->m_hasPWM = true;
    }
    else if (function == Position::SeqDelay)
    {
        if (value < 0 || value > 15) //This must be true.
        {
            qDebug() << "The delay value was invalid"
                     << " in " <<
                     "Position::addAdvancedPosition(SpecialFunction function, quint8 value)"
                     << " line: " << __LINE__;
            return false;
        }
        this->m_data.insert(function,value);
        this->m_hasDelay = true;
    }
    else
    {
        qDebug() << "Invalid special functions"
                 << " in "
                 << "Position::addAdvancedPosition(SpecialFunction function, quint8 value)"
                 << " line: " << __LINE__;
        return false;
    }
    return true;
}
/*
 *This acts the same as addAdvancedPosition except that it will treat the
 *value as an index instead of a user visible string if the special function
 *is PWMRepeat
 *
 *It returns true if the value was successfully added, false otherwise.
 */
bool Position::addAdvancedPositionIndex(SpecialFunction function, quint8 index)
{
    if (function != Position::PWMRepeat)
    {
        return this->addAdvancedPosition(function,index);
    }
    if (!this->m_PWMRepeatMap.contains(index))
    {
        qDebug() << "There was an invaid index to a PWMRepeat passed in."
                 << " In "
                 <<"Position::addAdvancedPositionIndex(SpecialFunction function, quint8 index)"
                 << " line: " << __LINE__;
        return false;
    }
    return this->addAdvancedPosition(function,this->m_PWMRepeatMap.value(index));
}
/*
  *This sets whether a freeze command is used for this postions.
  */
void Position::setFreeze(bool newFreeze)
{
    this->m_isFreeze = newFreeze;
}
/*
 *This shows if a freeze command is set for this position
 */
int Position::getBoardNumber()
{
    return this->m_boardNumber;
}
/*
 *This is used to set the board number that this position will addressed
 *to. It returns true if the boardNumber is within the valid range and
 *set, other wise it will return false.
 */
bool Position::setBoardNumber(int boardNumber)
{
    if (boardNumber > 7 || boardNumber < 1)//must fit in 3 bits.
    {
        return false;
    }
    this->m_boardNumber = boardNumber;
    return true;
}
/*
 *Checks if there has been data stored in this position. It does
 *not matter if there are positions or special functions.
 *Returns true if something is stored, returns false otherwise.
 */
bool Position::isEmpty()
{
    int size = this->m_data.size();
    int numSpec(0);
    if (this->m_data.contains(Position::PWMRepeat))
    {
        numSpec++;
    }
    if (this->m_data.contains(Position::PWMSweep))
    {
        numSpec++;
    }
    if (this->m_data.contains(Position::SeqDelay))
    {
        numSpec++;
    }
    if (size - numSpec <= 0)
    {
        return true;
    }
    return false;
}
/*
 *This returns the delay value if there is a delay value stored for
 *this position, otherwise it returns 0.
 */
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
/*
 *This is an overloaded method for using adding a new servo position
 *if there is no interest in a possible overwrite.
 */
bool Position::addServoPosition(quint8 servoNum, quint8 servoPosition)
{
    bool t;
    return this->addServoPosition(servoNum,servoPosition,t);
}
/*
 *This will add a new servo value to the positions. If the addition is successful
 *it will return true, otherwise it will return false.
 *
 *If another value already exists for a given address, then it will be overwritten
 *and the overwrite parameter will be set to true. Otherwise overwrite will be set
 *to false.
 */
bool Position::addServoPosition(quint8 servoNum, quint8 servoPosition, bool &overwrite)
{
    overwrite = false;
    if (servoNum > 12 || servoNum < 1) //Make sure the servo number is in range
    {
        qDebug() <<  "Error adding data to position: servo number out of range"
        << " in "
        << "Position::addServoPosition(quint8 servoNum, quint8 servoPosition, bool &overwrite)"
        << " line: " << __LINE__;
        return false;
    }
    if (servoPosition > 97 || servoPosition < 1) //Make the position is in range.
    {
        qDebug() <<  "Error adding data to position: servo position out of range"  << " in "
        << "Position::addServoPosition(quint8 servoNum, quint8 servoPosition, bool &overwrite)"
        << " line: " << __LINE__;
        return false;
    }
    if (this->m_data.contains(servoNum))//check for a possible overwrite
    {
        overwrite = true;
    }
    m_data.insert(servoNum,servoPosition);
    return true;
}
/*
 *This returns true if there is data for a servo number servoNumber stored
 *and returns true if there is. If the servoNumber is out of range or there
 *is no data stored it returns false.
 */
bool Position::hasPositonDataFor(int servoNumber)
{
    if (servoNumber > 12 || servoNumber < 1)
    {
        return false;
    }
    if (this->m_data.contains(servoNumber))
    {
        return true;
    }
    else
    {
        return false;
    }
}
/*
 *This gets the position data for a given servo number. If data is found it returns
 *the value. If there is no stored data or the servoNumber is out of range it returns
 * -1.
 */
int Position::getPositionDataFor(int servoNumber)
{
    if (servoNumber > 12 || servoNumber < 1)
    {
        return -1;
    }
    if (!this->hasPositonDataFor(servoNumber))
    {
        return -1;
    }
    return this->m_data.value(servoNumber);
}

/*Private Methods*/
/*
 *This forms the start of an output string based on the stored data.
 *The return will include a start symbol and PWM data if it is needed.
 */
QString Position::createStartOfString()
{
    QString output = "";
    if (m_isFreeze) //Choose the start symbol
    {
        output.append("&");
    }
    else
    {
        output.append("*");
    }
    //There is no sense in having sweep without repeat, but since they can be
    //added seperately, this allows for the problem to be solved on write/read
    //cycles.
    if (m_hasPWM && m_data.contains(PWMRepeat))
    {
        output.append(QString("PWMRep,%1")
                      .arg(m_PWMRepeatMap.value(m_data.value(PWMRepeat)),3,10,QLatin1Char('0')));
        if (m_hasPWM && m_data.contains(PWMSweep))
        {
            this->addTerminatingComma(output);
            output.append(QString("PWMSweep,%1")
                          .arg(m_data.value(PWMSweep),3,10,QLatin1Char('0')));
        }
    }
    return output;
}
/*
 *This will add a comma to the end of a string if one will be needed before
 *inserting anothe servo position or sequence delay.
 */
void Position::addTerminatingComma(QString& string)
{
    //This is the only case that you need to add a comma.
    if (!string.endsWith(',') && !string.endsWith('*') && !string.endsWith('&'))
    {
        string.append(",");
    }
}
/*
 *This will parse all the servo positions from an input string and store
 *the values before removing them from the input. If the parsing was
 *successful it will return true. Otherwise it will log the failure and
 *return false, this included the case where no servo positions are present
 *
 */
bool Position::parseServoPositions(QStringList &input)
{
    //Each line must have at least one servo position.
    if (input.isEmpty())
    {
        qDebug() << "Error parsing input string: no servo positions in string"
                 << " in " << "Position::parseServoPositions(QStringList &input)"
                 << " line: " << __LINE__;
        return false;
    }
    bool hasPositions = false;
    //Walk through the string until the end of servo positions section.
    while(!input.isEmpty() && input.at(0) != "SeqDelay")
    {
        bool ok = false;
        quint8 servoNum = input.at(0).toUShort(&ok,10);
        if (!ok || servoNum < 0 || servoNum > 12)
        {
            qDebug()<< "Error parsing servo address: value out of range or wrong format"
                       << " in " << "Position::parseServoPositions(QStringList &input)"
                       << " line: " << __LINE__;
            return false;
        }
        ok = false;
        quint8 servoData = input.at(1).toUShort(&ok,10);
        if (!ok || servoData < 1 || servoData > 97)
        {
            qDebug() << "Error parsing servo data: value out of range or wrong format"
                        << " in " << "Position::parseServoPositions(QStringList &input)"
                        << " line: " << __LINE__;;
            return false;
        }
        m_data.insert(servoNum,servoData);//Store the values after the parsing is successful
        hasPositions = true;
        input.removeFirst();
        input.removeFirst();
    }
    if (hasPositions)//
    {
        return true;
    }
    else
    {
        qDebug() << "Error parsing no servo data found in input string"
                    << " in " << "Position::parseServoPositions(QStringList &input)"
                    << " line: " << __LINE__;
        return false;
    }

}
/*
 *This will parse the begining of a string, the feeze option and the PWM options,
 *and store the values. It will return true unless there is an error in which case
 *it will log the error before returning false.
 *
 *The parameter info will be have up to its first 4 elements parsed and dropped
 *
 */
bool Position::parseStartOfString(QStringList& info)
{
    this->m_hasPWM = false;
    if (info.at(0) == "PWMRep") //See if there is a repeat, no sweep without repeat
    {
        bool ok = false;
        quint8 data = info.at(1).toUShort(&ok,10);
        if (!ok|| this->m_PWMRepeatMap.key(data,255) == 255)
        {
            qDebug() << "Error parsing PWM repeat: value out of range or wrong format in position"
                     << " in " << "Position::parseStartOfString(QStringList& info)"
                     << " line: " << __LINE__;
            return false;
        }
        data = m_PWMRepeatMap.key(data);
        info.removeFirst(); //Get rid of the stuff that has been handled
        info.removeFirst();
        if (info.at(0) == "PWMSweep") //If there is PWMRep, there must be sweep as well.
        {
            bool ok = false;
            quint8 data = info.at(1).toUShort(&ok,10);
            if (!ok || data > 15 || data < 0)
            {
                qDebug() << "Error parsing PWM sweep: value out of range or wrong format in position"
                         << " in " << "Position::parseStartOfString(QStringList& info)"
                         << " line: " << __LINE__;
                return false;
            }
            this->m_data.insert(Position::PWMSweep,data);
            info.removeFirst();
            info.removeFirst();
        }
        this->m_data.insert(Position::PWMRepeat,data); //Don't store until this is successful.
        this->m_hasPWM = true;
        return true;
    }
    this->m_hasPWM = false;
    return true;
}
/*
 *This is a utility method used to clear a specific bit in a byte without changing
 *any of the other bits.
 *It uses a mask and a bitwise AND since x & 1 = x
 */
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
/*
 *This is a utility method used to set a specific bit in a byte without changing
 *any of the other bits.
 *It uses a mask and a bitwise OR since x | 0 = x
 */
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


