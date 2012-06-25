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
    void fromStringInvalid();
    void fromStringInvalid_data();
    void fromStringValidSweep();
    void fromStringValidSweep_data();
    void fromStringInvalidSweep();
    void fromStringInvalidSweep_data();

    void stringInverses();
    void stringInverses_data();


    void isEmpty();
    void cleanupTestCase();
private:
    Position* p;
    
};
//Allows for positions to be passed from _data to test methods.
 Q_DECLARE_METATYPE ( Position )

#endif // TST_POSITION_H
