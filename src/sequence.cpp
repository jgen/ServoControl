#include "sequence.h"

Sequence::Sequence(QObject *parent) :
    QObject(parent),
    m_positions(),
    m_sequenceReplay(0),
    m_PWMRepeat(0),
    m_PWMSweep(0),
    m_sequenceDelay(1),
    m_runFormat(0),
    m_databank(0),
    m_replayMap(),
    m_comments(),
    m_hasData(false),
    m_iterator(0)
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
        m_positions.remove(0);
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
QString Sequence::toString(bool* okay)
{
    if (!this->m_hasData)
    {
        qDebug() << "Cannot convert sequence to string, there is no data";
        QString t;
        if(okay) *okay = false;
        return t;
    }
    QString outputString = "";
    QTextStream output(&outputString);
    int lineNumber = 0;
    for (Positions::iterator i = m_positions.begin() ; i != m_positions.end(); ++i)
    {
        while (m_comments.contains(lineNumber++))
        {
            output << m_comments.value(lineNumber) << endl;
        }

        output << (*i)->toString() << endl;
    }
    if (okay) *okay = true;
    output.flush();
    qDebug() << outputString;
    return outputString;

}



bool Sequence::fromString(QString data)
{
    QTextStream stream(&data,QIODevice::ReadOnly | QIODevice::Text);
    this->m_positions.clear();
    int lineNumber = 0;
    while(!stream.atEnd())
    {
        lineNumber++;
        QString line(stream.readLine());
        if(line.startsWith('#'))//Comment line
        {
            m_comments.insert(lineNumber,line);
            continue;
        }
        else if(line.startsWith('*') || line.startsWith('&'))
        {
            Position* p = new Position();
            if (!p->fromString(line))
            {
                qDebug() << tr("Failed parsing line: %1").arg(lineNumber);
                return false;
            }
            m_positions.append(p);
            continue;
        }
        else
        {
            qDebug() << tr("Error parsing line number %1 in the file").arg(lineNumber);
        }

    }
    m_hasData = true;
    return true;

}
bool Sequence::isVaild(QString data)//I hate the duplications, if you can find a better way do it.
{
    QTextStream stream(&data,QIODevice::ReadOnly | QIODevice::Text);
    int lineNumber = 0;
    while(!stream.atEnd())
    {
        lineNumber++;
        QString line(stream.readLine());
        if(line.startsWith('#'))//Comment line
        {
            continue;
        }
        else if(line.startsWith('*') || line.startsWith('&'))
        {
            Position* p = new Position();
            if (!p->fromString(line))
            {
                qDebug() << tr("Failed parsing line: %1").arg(lineNumber);
                return false;
            }
            continue;
        }
        else
        {
            qDebug() << tr("Error parsing line number %1 in the file").arg(lineNumber);
        }

    }
    return true;

}

bool Sequence::toFile(QFile &outputFile)
{
    if(!outputFile.isOpen())
    {
        if (!outputFile.open(QFile::ReadWrite | QFile::Text))
        {
            qDebug() << tr("Sequence::toFile failed to out the output file");
            return false;
        }
    }
    QTextStream output(&outputFile);
    bool ok = false;
    output << this->toFileString(&ok);
    if (!ok)
    {
        qDebug() << tr("Sequence::toFile failed on call to toFileString: invalied internal state");
        return false;
    }
    output.flush();
    outputFile.close();
    return true;
}

bool Sequence::toFile(QString outputFileName)
{
    QFile output(outputFileName);
    return this->toFile(output);
}

bool Sequence::fromFile(QString inputFileName)
{
    QFile f(inputFileName,this);
    return this->fromFile(f);
}

bool Sequence::fromFile(QFile &inputFile)
{
    if (!inputFile.isOpen())
    {
        if (!inputFile.open(QFile::ReadOnly | QFile::Text))
        {
            qDebug() << tr("Sequence::fromFile failed on opening the file");
            return false;
        }
    }
    QTextStream input(&inputFile);
    if (!this->fromFileString(input))
    {
        qDebug() << tr("Sequence::fromFile failed on call to fromFileString");
        return false;
    }
    inputFile.close();
    return true;

}

void Sequence::addPosition(Position* newPosition)
{
    this->m_positions.append(newPosition);//We need a copy constructor for positions.
    m_hasData = true;
}

void Sequence::resetIterator()
{
    this->m_iterator = 0;
}

bool Sequence::hasNext()
{
    if (m_iterator >= this->m_positions.size() )//Iterator points to the next to send.
    {
        return false;
    }
    return true;
}

int Sequence::getNextDelay()
{
    if (!this->hasNext())
    {
        qDebug() << tr("There are no more positions in the sequence");
        return 0;
    }
    int delay = this->m_positions.at(m_iterator)->getDelay();
    if (delay > 0)
        return delay;
    else return 1;//This is temporary until I get a way to deal with
    //global sequence delays.
}

QByteArray Sequence::getNextData()
{
    if (!this->hasNext())
    {
        qDebug() << tr("There is no more sequence data.");
        return QByteArray();
    }
   return this->m_positions.at(m_iterator++)->toServoSerialData();
}

/*Private Methods*/
bool Sequence::parseFileHeader(QString &header)
{
    QStringList data = header.split(',',QString::SkipEmptyParts);
    if (data.size() != 6)
    {
        qDebug() << tr("Error parsing file header: Wrong number of fields");
        return false;//Header is not in the right format.
    }
    if (!this->ParseRangeStore(data.at(0),m_runFormat,0,4))
    {
        qDebug() << tr("Error parsing file header: Run format out of range or not a number");
        return false;//Wrong format for runFormat
    }
    if (!this->ParseRangeStore(data.at(1),m_databank,0,5))
    {
        qDebug() << tr("Error parsing file header: Databank out of range or not a number");
        return false; //Wrong format for databank.
    }
    if (!this->ParseRangeStore(data.at(2),m_PWMRepeat,0,7))
    {
        qDebug() << tr("Error parsing file header: PWM repeat out of range or not a number");
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

QString Sequence::headerToString()
{
    if(!this->m_sequenceDelay)
    {
        this->m_sequenceDelay = 1; //Can never have 0 sequence delay.
    }
    QString out = "";
    out += QString("%1,").arg(this->m_runFormat,3,10,QLatin1Char('0'));
    out += QString("%1,").arg(this->m_databank,3,10,QLatin1Char('0'));
    out += QString("%1,").arg(this->m_PWMRepeat,3,10,QLatin1Char('0'));
    out += QString("%1,").arg(this->m_PWMSweep,3,10,QLatin1Char('0'));
    out += QString("%1,").arg(this->m_sequenceDelay,3,10,QLatin1Char('0'));
    out += QString("%1").arg(this->m_sequenceReplay,3,10,QLatin1Char('0'));
    /*
    QString out = QString("%1,%2,%3,%4,%5,%6").arg(this->m_runFormat,3,10,QLatin1Char('0'))
                                    .arg(this->m_databank,3,10,QLatin1Char('0'))
                                    .arg(this->m_PWMRepeat,3,10,QLatin1Char('0'))
                                    .arg(this->m_PWMRepeat,3,10,QLatin1Char('0'))
                                    .arg(this->m_sequenceDelay,3,10,QLatin1Char('0'))
                                    .arg(this->m_sequenceReplay,3,10,QLatin1Char('0'));*/
    return out;
}
QString Sequence::toFileString(bool* okay)
{
    if (!this->m_hasData)
    {
        qDebug() << "Cannot convert sequence to string, there is no data";
        QString t;
        if(okay) *okay = false;
        return t;
    }
    QString outputString = "";
    QTextStream output(&outputString);
    output << this->headerToString() << endl;
    bool ok = false;
    output << this->toString(&ok);
    if (!ok)
    {
        qDebug() << "Sequence::toFileString(bool* okay) failed on call to Sequence::toString()";
    }
    return outputString;

}



bool Sequence::fromFileString(QTextStream& stream)
{

    int lineNumber = 0;
    while(!stream.atEnd())
    {
        lineNumber++;
        QString line(stream.readLine());
        if(line.startsWith('#'))//Comment line
        {
            m_comments.insert(lineNumber,line);
            continue;
        }
        else if(lineNumber == 1 && line.startsWith('0'))//Only line that should to this is the first
        {                           //it cannot start with anything else
            if (!this->parseFileHeader(line))
            {
                qDebug() << tr("Failed parsing the header");
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
    m_hasData = true;
    return true;

}
