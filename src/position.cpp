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
        quint8 d = 2;
        m_data.insert(i,d);
    }
    qDebug() << this->toString();
}

/*Public Methods*/
bool Position::fromString(QString fileFormatString)
{
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
        output.append("PWMRep,");
        output.append(QString("%1").arg(m_PWMRepeatMap.value(m_data.value(PWMRepeat)),3,10,QLatin1Char('0')));
    }
    if (m_data.contains(PWMSweep))
    {
        this->addTerminatingComma(output);
        output.append("PWMSweep,");
        output.append(QString("%1").arg(m_data.value(PWMSweep),3,10,QLatin1Char('0')));
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

QByteArray Position::toSerialData()
{

}

