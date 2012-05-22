#include "sequence.h"

Sequence::Sequence(QObject *parent) :
    QObject(parent),
    m_positions(),
    m_sequenceReplay(0),
    m_PWMRepeat(0),
    m_PWMSweep(0),
    m_sequenceDelay(0),
    m_runFormat(0),
    m_databank(0),
    m_replayMap(),
    m_comments()
{
    this->init();
}

Sequence::~Sequence()
{
    Position* p;
    while(!this->m_positions.isEmpty())
    {
        p = m_positions.first();
        delete p;
    }
}

void Sequence::init()
{
    m_replayMap.insert(0,1);
    m_replayMap.insert(1,2);
    m_replayMap.insert(2,10);
    m_replayMap.insert(3,25);
    m_replayMap.insert(4,50);
    m_replayMap.insert(5,100);
    m_replayMap.insert(6,150);
    m_replayMap.insert(7,200);
}

/*Public Methods*/
QString Sequence::toString()
{

}

bool Sequence::fromString(QString data)
{
    QTextStream stream(&data,QIODevice::ReadOnly);
    int lineNumber = 0;
    while(stream.atEnd())
    {
        lineNumber++;
        QString line(stream.readLine());
        if(line.startsWith('#'))//Comment line
        {
            m_comments.insert(lineNumber,line);
            continue;
        }
        else if(line.startsWith('0'))//Only line that should to this is the first
        {                           //it cannot start with anything else
            if (!this->parseFileHeader(line))
            {
                qDebug() << "Failed parsing the header";
                return false; //failed parsing the header.
            }
            continue;
        }
        else if(line.startsWith('*') || line.startsWith('&'))
        {
            Position* p = new Position();
            p->fromString(line);
            m_positions.append(p);
            continue;
        }
        else
        {
            qDebug() << tr("Error parsing line number %1 in the file").arg(lineNumber);
        }

    }
}

bool Sequence::parseFileHeader(QString &header)
{
    QStringList data = header.split(',',QString::SkipEmptyParts);
    if (data.size() != 6)
    {
        return false;//Header is not in the right format.
    }
    if (!this->ParseRangeStore(data.at(0),m_runFormat,0,4))
    {
        return false;//Wrong format for runFormat
    }
    if (!this->ParseRangeStore(data.at(1),m_databank,0,5))
    {
        return false; //Wrong format for databank.
    }
    if (!this->ParseRangeStore(data.at(2),m_PWMRepeat,0,7))
    {
        return false; //Wrong PWM repeat format
    }
    if (!this->ParseRangeStore(data.at(3),m_PWMSweep,0,15))
    {
        qDebug() << tr("Header: PWM sweep value what is the wrong format");
        return false;
    }
    if (!this->ParseRangeStore(data.at(4),m_sequenceDelay,0,15))
    {
        qDebug() << tr("Header: Sequence Delay is in the wrong format");
        return false;
    }
    if (!this->ParseRangeStore(data.at(5),m_sequenceReplay,0,7))
    {
        qDebug() << tr("Header: Sequence replay is in the wrong format");
    }
    return true;
}
inline bool Sequence::ParseRangeStore(const QString& source, quint8& dest, int min, int max)
{
    bool ok = false;
    quint8 temp = source.toUShort(&ok,10);
    if (!ok || temp < min || temp > max)
    {
        return false;
    }
    dest = temp;
    return true;
}

bool Sequence::toFile(QFile &outputFile)
{

}

bool Sequence::fromFile(QFile &intputFile)
{

}

void Sequence::addPosition(Position* newPosition)
{
    this->m_positions.append(newPosition);//We need a copy constructor for positions.
}

