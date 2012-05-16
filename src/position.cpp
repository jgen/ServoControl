#include "position.h"

Position::Position(QObject *parent) :
    QObject(parent)
{
    init();
}
void Position::init()
{
}

/*Public Methods*/
bool Position::fromString(QString fileFormatString)
{
}
QString Position::toString()
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
        output.append(m_data.value(PWMRepeat));
        output.append(",");
    }
    if (m_data.contains(PWMSweep))
    {
        if (!output.endsWith(",") || !output.endsWith("*") || !output.endsWith("&"))
        {
            output.append(",");
        }
        output.append("PWMSweep,");
        output.append(m_data.value(PWMSweep));
        output.append(",");
    }
}
QByteArray Position::toSerialData()
{

}

