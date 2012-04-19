#include "position.h"

Position::Position(QObject *parent) :
    QObject(parent)
{
    init();
}
void Position::init()
{
    for (int i = 0; i < 12; i++)
    {
        positions[i] = 0;
    }
    this->freeze = false;
}

/*Public Methods*/
bool Position::fromString(QString fileFormatString)
{
    if(fileFormatString.at(0) == '*')
    {
        fileFormatString.remove('*');
    }
    QStringList token = fileFormatString.split(",");
    if(token.length() % 2 != 0)
    {
        return false;

    }
    while(!token.isEmpty())
    {
        positions[token.at(0).toInt()] = token.at(1).toInt();
        token.removeFirst();
        token.removeFirst();
    }
    return true;
}
QString Position::toString()
{
    QString result;
    for (int i = 0; i < 12; i++)
    {
        if (positions[i] == 0)
        {
            continue;
        }
        result.append( QString(",%1,").arg(i));
        switch(positions[i])
        {
        case 0:
            result.append( QString("00%1").arg(positions[i]));
            break;
        case 10:
            result.append( QString("0%1").arg(positions[i]));
        default:
            break;

        }


    }
}
QByteArray Position::toSerialData()
{

}

