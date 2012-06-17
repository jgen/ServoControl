#ifndef TST_POSITION_H
#define TST_POSITION_H

#include <QObject>
#include <QTest>
#include "position.h"

class tst_Position : public QObject
{
    Q_OBJECT
public:
    explicit tst_Position(QObject *parent = 0);
    
signals:
    
public slots:

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void addPositionRejectInvalid();
    void addPositionRejectInvalid_data();
    void addPositionOverwrite();
    void addPositionOverwrite_data();
    void cleanupTestCase();
private:
    Position* p;
    
};

#endif // TST_POSITION_H
