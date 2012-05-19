#include "position.h"

Position::Position(QObject *parent) :
        QObject(parent),
        m_PWMRepeatMap(),
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
    qDebug() << this->fromString(this->toString());
    qDebug() << this->toString();
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

QString Position::toString()
{
    QString output = createStartOfString();
    for(int i = 1; i <= 12; i++)
    {
        if(m_data.contains(i))
        {
            this->addTerminatingComma(output);
            output.append(QString("%1,%2").arg(i,3,10,QLatin1Char('0'))
                          .arg(m_data.value(i),3,10,QLatin1Char('0')));
        }
    }
    if(m_data.contains(Position::SeqDelay))
    {
        this->addTerminatingComma(output);
        output.append(QString("SeqDelay,%1").arg(m_data.value(SeqDelay),3,10,QLatin1Char('0')));
    }
    return output;

}

QByteArray Position::toSerialData()
{

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


