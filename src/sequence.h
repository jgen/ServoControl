#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <QObject>
#include <QFile>
#include <QVector>
#include "position.h"

typedef  QVector<Position> Positions;//Not sure what is best, can figure that out later.
class Sequence : public QObject
{
    Q_OBJECT
public:
    explicit Sequence(QObject *parent = 0);
    QString toString();
    bool fromString(const QString data); //copy on write, so no need to use a reference
    bool toFile(QFile& outputFile);
    bool fromFile(QFile& intputFile);

    //TODO: Add stream operators for string and file stream.

signals:

public slots:

private:
    Positions m_positions;

};

#endif // SEQUENCE_H
