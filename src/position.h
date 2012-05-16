#ifndef POSITION_H
#define POSITION_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>
#include <QMap>
class Position : public QObject
{
    Q_OBJECT
public:
    enum SpecialFunction {PWMRepeat = 13, PWMSweep, SeqDelay};
    explicit Position(QObject *parent = 0);


    QByteArray toSerialData();
    QString toString();//Human readable string
    bool fromString(QString input);
signals:

public slots:

private:
    void init();
    QMap<quint8,quint8> m_data;
    bool m_isFreeze;

};

#endif // POSITION_H
