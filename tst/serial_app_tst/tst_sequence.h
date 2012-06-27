#ifndef TST_SEQUENCE_H
#define TST_SEQUENCE_H

#include <QObject>
#include <QTest>

#include "sequence.h"

class Position;

class tst_Sequence : public QObject
{
    Q_OBJECT
public:
    explicit tst_Sequence(QObject *parent = 0);
    
signals:
    
public slots:

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    void testAddStartPosition();



private:
    Sequence* s;

    void addPositions();
    
};

#endif // TST_SEQUENCE_H
