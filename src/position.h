#ifndef POSITION_H
#define POSITION_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QByteArray>
class Position : public QObject
{
    Q_OBJECT
public:
    explicit Position(QObject *parent = 0);

    QByteArray toSerialData();
    QString toString();//Human readable string
    bool fromString(QString input);
signals:

public slots:

private:
    void init();
    int positions[12];
    bool freeze;
};

#endif // POSITION_H
