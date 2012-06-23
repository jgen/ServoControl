#ifndef TST_POSITION_H
#define TST_POSITION_H

#include <QObject>
#include <QTest>
#include <QMap>

#include "position.h"

typedef QMap<quint8,quint8> DataPair;//Work around since you can't use mulitple parameter
Q_DECLARE_METATYPE(DataPair)         //templates in QFETCH

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

    void addPositionRejectInvalid();//Try to add invalid servo values
    void addPositionRejectInvalid_data();
    void addPositionOverwrite();//Try to overwrite previously stored values
    void addPositionOverwrite_data();
    void toServoData();
    void toServoData_data();
    void toPWMServoData();
    void toPWMServoData_data();
    void hasDataFor();
    void hasDataFor_data();

    void fromStringValid();
    void fromStringValid_data();
    void fromStringInvalid_data();
    void fromStringInvalid();
//Invalid data and special cases, comment lines.
    void isEmpty();
    void cleanupTestCase();
private:
    Position* p;
    
};

#endif // TST_POSITION_H
