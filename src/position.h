#ifndef POSITION_H
#define POSITION_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QMap>
#include <QDebug>

/*
 Storage Notes: Store PWMRepeat and SequenceReplay as 0-->15, use lookup for actual value.
*/
class Position : public QObject
{
    Q_OBJECT
public:
    enum SpecialFunction {PWMRepeat = 13, PWMSweep = 14, SeqDelay = 15};
    explicit Position(QObject *parent = 0);


    QByteArray toSerialData();
    QString toString();//Human readable string
    bool fromString(QString input);
signals:

public slots:

private:
    void init();
    void addTerminatingComma(QString& string);
    QString createStartOfString();

    QMap<quint8,int> m_PWMRepeatMap;
    QMap<quint8,quint8> m_data;
    bool m_hasData;
    bool m_isFreeze;
    bool m_hasDelay;
    bool m_hasPWM;

};

#endif // POSITION_H
