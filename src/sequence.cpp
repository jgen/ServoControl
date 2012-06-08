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
    return this->toString(okay,false);
}



bool Sequence::fromString(QString data)
{
    if (data.isEmpty())
    {
        qDebug() << "Sequnce::fromString - the string was empty";
        return false;
    }
    QTextStream stream(&data,QIODevice::ReadOnly | QIODevice::Text);
    this->m_positions.clear();
    this->m_comments.clear();
    int lineNumber = 0;
    while(!stream.atEnd())
    {
        lineNumber++;
        QString line(stream.readLine());
        if (line.trimmed() == "")//To deal with empty lines, they are removed when stored
        {
            lineNumber--;
            continue;
        }
        if(line.trimmed().startsWith('#'))//Comment line
        {
            m_comments.insert(lineNumber,line.trimmed());
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
            qDebug() << "this si the error";
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
    bool legacyMode;
    if (outputFile.fileName().endsWith(".SER"))
    {
        legacyMode = true;
    }
    else //If they choose their own file type they get the new stuff.
    {
        legacyMode = false;
    }

    output << this->toFileString(&ok,legacyMode);
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

bool Sequence::setDelay(quint8 delay)
{
    if (delay < 1 || delay > 15)
    {
        qDebug() << tr("The delay value is out of range");
        return false;
    }
    this->m_sequenceDelay = delay;
    return true;
}
bool Sequence::setReplay(quint8 replay)
{
    if (this->m_replayMap.key(replay,-1) == -1)
    {
        qDebug() << tr("Invalid replay value: &1").arg(replay);
        return false;
    }
    this->m_sequenceReplay = this->m_replayMap.key(replay);
    return true;
}

int Sequence::getRepeats()
{
    return this->m_replayMap.value(this->m_sequenceReplay,0);
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
    {
        return delay;
    }
    else
    {
        return this->m_sequenceDelay;//This is temporary until I get a way to deal with
        //global sequence delays.
    }


}

QByteArray Sequence::getNextData()
{
    if (!this->hasNext())
    {
        qDebug() << tr("There is no more sequence data.");
        return QByteArray();
    }
    QByteArray retVal;
    if (this->m_positions.at(m_iterator)->hasPWMData())
    {
        retVal.append(this->m_positions.at(m_iterator)->getPWMSerialData());
    }
    retVal.append(this->m_positions.at(m_iterator++)->toServoSerialData());
    return retVal;
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
QString Sequence::toFileString(bool* okay,bool legacyMode)
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
    output << this->toString(&ok,legacyMode);
    if (!ok)
    {
        *okay = false;
        qDebug() << "Sequence::toFileString(bool* okay) failed on call to Sequence::toString()";
    }
    *okay = true;
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

QString Sequence::toString(bool *okay, bool legacyFormat)
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
        lineNumber++;
        while (m_comments.contains(lineNumber))
        {
            if(!legacyFormat)

            {
                output << m_comments.value(lineNumber++) << endl;
            }
            else
            {
                lineNumber++;
            }
        }
        output << (*i)->toString(legacyFormat) << endl;
    }
    if (okay) *okay = true;
    output.flush();
    return outputString;

}
