#ifndef TST_SEQUENCE_H
#define TST_SEQUENCE_H

#include <QObject>
#include <QTest>

#include "sequence.h"
#include "position.h"

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
    void testAddStartPositionInvalid();
    void testAddStartPositionInvalid_data();

    //From text - along with every combination of junk.
    //from and to string are inverses

    //Check that the iterator works, and that it moves/resets correctly



private:
    Sequence* s;

    void addPositions();
    
};
Q_DECLARE_METATYPE ( Position*)


#endif // TST_SEQUENCE_H
