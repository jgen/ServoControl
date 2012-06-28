#include "sequence.h"

/*Constructs the object and sets the default values.*/
Sequence::Sequence(QObject *parent) :
    QObject(parent),
    m_positions(),
    m_sequenceReplay(0),
    m_PWMRepeat(0),
    m_PWMSweep(0),
    m_sequenceDelay(1),//This can never be zero
    m_runFormat(0),
    m_databank(0),
    m_replayMap(),
    m_comments(),
    m_hasData(false),
    m_iterator(0),
    m_startPosition(0),
    m_hasStartPosition(false)
{
    this->init();
}
/*! Cleans up any memory, note that if there is a shared pointer to a
 *  position it will will be invalid after this call, so don't do that.
 */
Sequence::~Sequence()
{
    Position* p;
    while(!this->m_positions.isEmpty())
    {
        p = m_positions.first();
        delete p;
        m_positions.remove(0);
    }
    if(this->m_startPosition)
    {
        delete m_startPosition;
    }
}
/*! Set up the map to emulate the lookup tables on the device.*/
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

/*!
 *Gets the user visible string representation for the data stored.
 *The okay parameter tells if the string returned is valid.
 * \param okay
 *        Will be set true if the conversion succeeded.
 * \return A user visible string with the positions and comments.
 */
QString Sequence::toString(bool* okay)
{
    return this->toString(okay,false);
}


/*!
 *Parses a user visible string and stores the results. It will
 *return true iff the string is successfully parsed and the sequence
 *is in a valid internal state.
 * \param data
 *      A user visible string (only positions and comments)
 *      that will be used to intialize the object.
 * \return True if the parsing of the string succeeded.
 */
bool Sequence::fromString(QString data)
{
    if (data.isEmpty()) //Will not null initalize on empty string.
    {
        qDebug() << "Sequnce::fromString - the string was empty";
        return false;
    }
    QTextStream stream(&data,QIODevice::ReadOnly | QIODevice::Text);
    if (!this->m_positions.isEmpty())//Reinialize the data being stored.
    {
        Position* p;
        while(!this->m_positions.isEmpty())
        {
            p = m_positions.first();
            delete p;
            m_positions.remove(0);
        }
    }
    this->m_comments.clear();//Reinialize the comments being stored.
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
            m_comments.insert(lineNumber,line.trimmed());//Store without the whitespace.
            continue;
        }
        else if(line.startsWith('*') || line.startsWith('&'))//Position line
        {
            Position* p = new Position();
            if (!p->fromString(line)) //Position will parse the line.
            {
                qDebug() << tr("Failed parsing line: %1").arg(lineNumber);
                return false;
            }
            m_positions.append(p);
            continue;
        }
        else //Do not allow unreconized lines in the file.
        {
            qDebug() << tr("Error parsing line number %1 in the file").arg(lineNumber);
        }
    }
    m_hasData = true;
    return true;

}
/*!
 * This is used to check if a user visible string is valid. It will not store
 * any of the data from the string. It can be used without changing the internal
 * state of the object.
 * \param data
 *       This is a user visible string that will be checked for correct formatting.
 * \return True is the string is valid, otherwise false.
 */
bool Sequence::isVaild(QString data)//I hate the duplications, if you can find a better way, do it.
{
    if (data.isEmpty())
    {
        return false;
    }
    QTextStream stream(&data,QIODevice::ReadOnly | QIODevice::Text);
    int lineNumber = 0;
    while(!stream.atEnd())
    {
        lineNumber++;
        QString line(stream.readLine());
        if (line.isEmpty())
        {
            continue;
        }
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
            delete p;//Cannot store the result.
            continue;
        }
        else
        {
            qDebug() << tr("Error parsing line number %1 in the file").arg(lineNumber);
            return false;
        }

    }
    return true;

}
/*!
 * This will write the sequence to the give output file using the file strings. The
 * choice of to use legacy format is made based off the file name, if the extension is
 * .SER it will use  the legacy format. Otherwise the newer format will be used.
 *
 * It does not matter if the file is open when it is passed in, but it will be closed
 * when it is returned.
 * \param outputFile
 *    This is the file that the sequence will be written to. It does not have to be open
 *    when passed in, but it will be closed afterwards.
 * \return True if the file successfully written to with the sequence data. If the file
 *    is invalid or there is an issue with the state of the sequence it will return false.
 */
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
/*!
 * This is a simple overload for writing to a file if only the filename is known.
 * It will handle opening and closing the file, as well as the writing to it.
 * \param outputFileName
 *      This must be a file name and path, complete with the file extension, where
 *      the file will be written to. The extension given will be read to choose
 *      which format will be written. The path can be either relative or absolute.
 *      If the file doesn't exist it will be created.
 * \return True if the sequence is successfully written to the file, false otherwise.
 */
bool Sequence::toFile(QString outputFileName)
{
    QFile output(outputFileName);
    return this->toFile(output);
}
/*!
 * This is a convience function for opening a file if only the filename to read from is given.
 * It will handle the opening and closing of the file, as well as reading from it. The sequence
 * will be initalized based on the information in the file. If the file is invalid, the sequence
 * will be in an undetermined state.
 * \param inputFileName
 *      This must be a file name and path, complete with the file extension, where
 *      the file will be written to. The extension given will be read to choose
 *      which format it will be read as. The path can be either relative or absolute.
 * \return True if the sequnce is successfully intialzed from the file, otherwise false.
 */
bool Sequence::fromFile(QString inputFileName)
{
    QFile f(inputFileName,this);
    return this->fromFile(f);
}
/*!
 * This reads a file and stores the parsed values if they are valid. The file
 * will be opened if needed, but will always be returned closed. The sequence
 * will be initalzed based on the information in the file if it is valid. If the
 * file is invalid the sequence will be in an undetermined state.
 * \param inputFile
 *       The file that will be read from.
 * \return True if the sequence is successfully initailzed from the file.
 */
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
/*
 *This is used to add another position to the sequence at the end. There is no
 *copy made of the position being passed in and the memory will be released later
 *so do not delete the value passed in.
 */
void Sequence::addPosition(Position* newPosition)
{
    this->m_positions.append(newPosition);//We need a copy constructor for positions.
    m_hasData = true;
}
/*
 *This resets the internal iterator back to the start for use with hasNext and getNext
 *functions.
 */
void Sequence::resetIterator()
{
    this->m_iterator = 0;
}
/*
 *This returns true if there is another position to be returned. This will not advance
 *the iterator and leaves the internal start unchanged.
 */
bool Sequence::hasNext()
{
    if (m_iterator >= this->m_positions.size() )//Iterator points to the next to send.
    {
        return false;
    }
    return true;
}
/*
 *This is used to set the delay between positions that will be used if there is no
 *delay in the position that is being read. This value will be written to the header
 *of any files that the sequence is saved to.
 */
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
/*
 *This sets the number of times that the sequence is repeated during playback. This
 *value will be written into the file header when the sequence is saved. The replay value
 *is the user visible value. (ie 200 not 7)
 */
bool Sequence::setReplay(quint8 replay)
{
    if (this->m_replayMap.key(replay,-1) == -1) //See if the value is valid
    {
        qDebug() << tr("Invalid replay value: &1").arg(replay);
        return false;
    }
    this->m_sequenceReplay = this->m_replayMap.key(replay);//Store the key, not the user visible
    return true;
}
/*
 *Returns the number of times that the sequence should be repeated when being played back.
 *If there is no value stored or an error occurs it will return 0.
 */
int Sequence::getRepeats()
{
    return this->m_replayMap.value(this->m_sequenceReplay,0);
}
/*
 *This is used to set the sequence PWM values that will be used if a line doensn't specify
 *what values to use. It returns true iff both values are valid and are stored. If either
 *value is invalid the state will no be changed.
 */
bool Sequence::setPWMValues(quint8 repeat, quint8 sweep)
{
    if (repeat > 8 || repeat < 0)//This (and below) give warnings on *nix, but better to be safe.
    {
        qDebug() << tr("Error adding global PWM values: PWM Repeat is out of range");
        return false; //Wrong PWM repeat format
    }
    if (sweep > 15 || sweep < 0)
    {
        qDebug() << tr("Error adding global PWM values: PWM sweep is out of range.");
        return false;
    }
    this->m_PWMRepeat = repeat;
    this->m_PWMSweep = sweep;
    return true;
}
/*
 *This will return the delay of the position that the iterator is currently pointing to.
 *If there there is no delay for the position it will return the sequence delay, and if
 *none has been set it will return the default value (1).
 */
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
        return this->m_sequenceDelay; //
    }
}
/**
 * \brief his returns the serial data for for the next position in the sequence and if
 *   needed a pointer to the position that was used.
 *
 * The freeze data will be included along with the PWM values. If there is no PWM
 * information for the position then
 * the global sequence values will be used, or failing that the PWM values will be
 * turned off.
 *
 * The position that is returned will have the memory automatically managed, deleting it
 * will cause the sequence to become in valid. If the position is needed for a longer time,
 * make a copy as the pointer will become invalid if the sequence is reinitialized at any time.
 */
QByteArray Sequence::getNextData(Position*& p)
{
    if (!this->hasNext())
    {
        qDebug() << tr("There is no more sequence data.");
        return QByteArray();
    }
    QByteArray retVal;
    if (this->m_positions.at(m_iterator)->hasPWMData())//if the position has data use it
    {
        retVal.append(this->m_positions.at(m_iterator)->getPWMSerialData());
    }
    else if (this->m_PWMRepeat != 0 && this->m_PWMSweep != 0)//Use the data we have
    {
        Position p;//Easier to use the positon method as opposed to re-writing it.
        p.addAdvancedPositionIndex(Position::PWMRepeat,this->m_PWMRepeat);
        p.addAdvancedPositionIndex(Position::PWMSweep,this->m_PWMSweep);
        retVal.append(p.getPWMSerialData());
    }
    else //Turn off sweeps if they are not requested explicitly.
    {
        Position p;
        p.addAdvancedPositionIndex(Position::PWMRepeat,0);
        p.addAdvancedPositionIndex(Position::PWMSweep,0);
        retVal.append(p.getPWMSerialData());

    }
    p = this->m_positions[m_iterator];
    retVal.append(this->m_positions.at(m_iterator++)->toServoSerialData());
    return retVal;
}

bool Sequence::hasStartPosition()
{
    return this->m_startPosition && this->m_hasStartPosition;
}

QByteArray Sequence::getStartPositionCommand()
{
    if (!this->hasStartPosition() && m_startPosition
            && !m_startPosition->hasPWMData())
    {
        return QByteArray();
    }
    QByteArray retVal;
    retVal.append((char)159);//Freeze
    retVal.append((char)15);
    retVal.append((char)156);//Write address
    retVal.append((char)00);
    this->m_startPosition->setFreeze(false);
    retVal.append(m_startPosition->toServoSerialData());
    retVal.append((char)156);//Write address
    retVal.append((char)00);
    retVal.append((char)159);//Store and reset micro
    retVal.append((char)13);
    return retVal;

}

bool Sequence::setStartPosition(Position* p)
{
    if (!p || p->hasPWMData() || p->getDelay())
    {
        qDebug() << "Pointer to set start position was null";
        return false;
    }
    if (this->hasStartPosition())
    {
        delete this->m_startPosition;
    }
    p->setFreeze(false);
    this->m_startPosition = p;
    this->m_hasStartPosition = true;
    return true;

}

Position* Sequence::getStartPosition()
{
    return this->m_startPosition;
}

bool Sequence::isEmpty()
{
    return this->m_positions.isEmpty() && this->m_comments.isEmpty();
}
void Sequence::clearStoredPositions()
{
    this->m_positions.clear();
    this->m_comments.clear();
    this->m_hasData = false;
}


/*Private Methods*/

/*
 *This parses the header from an input file. The header form is explained in the
 *File Format and Commands document. The format is the same across the new and old
 *formats so there is no need for lagacy mode flags.
 *
 *If any value is in the wrong format then the function will return false. If a value
 *is incorrect then there will be no way to determine the state of the objevt and it will
 *have to be reintialized.
 */
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
/*
 *This is used to simplify the parsing and storeing of values from a string. It parses the string
 *for and integet and then makes sure it is between min and max value before storing it in the
 *destination that is provided. If the parse fails or the values falls outside of the range it
 *will return false and not change the value of dest.
 */
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
/*
 *This writes the header string based on the current data stored in the sequence.
 */
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
/*
 *This returns the complete string that will form a valid file, including the header.
 *The lagacy mode flag is used to choose if the newer features (PWMSweeps, comments) are
 *included in the file. The okay flag is returned true if there are no problems and the
 *string is valid.
 */
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
    if (this->m_hasStartPosition && this->m_startPosition && !legacyMode)
    {
        output << "Start:" << m_startPosition->toString(true) << "\n";//Old school string format for starting
    }
    output << this->toString(&ok,legacyMode);
    if (!ok)
    {
        *okay = false;
        qDebug() << "Sequence::toFileString(bool* okay) failed on call to Sequence::toString()";
    }
    *okay = true;
    return outputString;

}


/*
 *This reads the given stream as though it is a file and stores the data. It will
 *reject the string if it is not a valid file format, and return false. The state
 *is undefined in that case and the sequence should be reinitalized.
 *
 *The reading is done assuming that it is a newer file format, and if it is the legacy
 *format there should be no difference in its behaviour.
 */
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
        else if (lineNumber == 2 && line.startsWith("Start:"))
        {
            this->m_startPosition = new Position();
            line.remove("Start:");
            if (!this->m_startPosition->fromString(line))
            {
                qDebug() << tr("Failed parsing file starting position");
                return false;
            }
            if (this->m_startPosition->hasPWMData())
            {
                qDebug() << tr("Start position had sweep data");
                return false;
            }
            for (int i(1); i <= 12; ++i)
            {
                if (!m_startPosition->hasPositonDataFor(i))
                {
                    qDebug() << tr("Start positoni was missing data for servo ") << i;
                    return false;
                }
            }

            this->m_hasStartPosition = true;
        }
        else if(line.startsWith('*') || line.startsWith('&')) //Position line
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
/*
 *This converts the sequence to a string that can be displayed to the user. If
 *there is an error the okay flag will be set to false and will return an empty
 *string. The legacy format is used to show if the new features should be used.
 *This flag should be set to false unless you are writing to an older file.
 */
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
