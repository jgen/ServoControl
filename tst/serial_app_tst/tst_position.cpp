#include "tst_position.h"

tst_Position::tst_Position(QObject *parent) :
    QObject(parent)
{
}
/*Work around to suppress qDebug statments from tested units*/

void mMessageOutput(QtMsgType type, const char *msg)
{
}
void tst_Position::initTestCase()
{
    qInstallMsgHandler(mMessageOutput);
}
void tst_Position::init()
{
    p = new Position();
}

void tst_Position::addPositionRejectInvalid_data()
{
    QTest::addColumn<quint8>("address");
    QTest::addColumn<quint8>("value");

    QTest::newRow("All values out of range") << (quint8)15 << (quint8)150;
    QTest::newRow("Address too high") << (quint8)15 << (quint8)45;
    QTest::newRow("Data too high") << (quint8)10 << (quint8)120;
    QTest::newRow("Data is zero") << (quint8)10 << (quint8)0;
}

void tst_Position::addPositionRejectInvalid()
{
    QFETCH(quint8,address);
    QFETCH(quint8, value);

    QVERIFY(!p->addServoPosition(address,value));

}
void tst_Position::addPositionOverwrite_data()
{
    QTest::addColumn<quint8>("address");
    QTest::addColumn<quint8>("position");
    QTest::addColumn<bool>("isOverwrite");
    QTest::addColumn<bool>("tryOverwrite");

    QTest::newRow("all values valid no overwrite") << (quint8)1 << (quint8)25 << (bool)false << (bool)false;
    QTest::newRow("all values valid overwrite") << (quint8)1 << (quint8)45 << (bool)true << (bool) true;
    QTest::newRow("invalid values")<< (quint8)1 << (quint8)100 << (bool)false << (bool) true;
}
void tst_Position::addPositionOverwrite()
{
    QFETCH(quint8,address);
    QFETCH(quint8,position);
    QFETCH(bool, isOverwrite);
    QFETCH(bool,tryOverwrite);
    bool overwrite = false;
    p->addServoPosition(address,position,overwrite);
    if (tryOverwrite) //There is a new position everytime so we need to write twice sometimes.
    {
        p->addServoPosition(address,position + 1,overwrite);
    }
    QVERIFY(overwrite == isOverwrite);
}
void tst_Position::toServoData_data()
{
    QTest::addColumn< DataPair >("data");
    QTest::addColumn<QByteArray>("output");
    DataPair forward;
    DataPair middle;
    DataPair backwards;
    Position p;
    DataPair mirror;
    DataPair sparseEven;
    DataPair sparseOdd;

    for (int i(1); i <= 12; ++i)
    {
        forward.insert(i,2);
        middle.insert(i,42);
        backwards.insert(i,96);
        mirror.insert(i,30);
        p.addServoPosition(i,30);
        if (i % 2 == 0)
        {
            sparseEven.insert(i,42);
        }
        else
        {
            sparseOdd.insert(i,42);
        }

    }
    QTest::newRow("All forward") << forward
                                 << QByteArray::fromHex("90029102920293029402950296029702980299029A029B02");
    QTest::newRow("All middle") << middle
                                << QByteArray::fromHex("902A912A922A932A942A952A962A972A982A992A9A2A9B2A");
    QTest::newRow("Backwards") << backwards
                               << QByteArray::fromHex("90609160926093609460956096609760986099609A609B60");
    QTest::newRow("mirroring") << mirror << p.toServoSerialData();
    QTest::newRow("Sparse Even") << sparseEven << QByteArray::fromHex("912A932A952A972A992A9B2A");
    QTest::newRow("Sparse Odd") << sparseOdd << QByteArray::fromHex("902A922A942A962A982A9A2A");
}

void tst_Position::toServoData()
{
    QFETCH(DataPair, data);
    QFETCH(QByteArray,output);

    for (int i(1); i <= 12; ++i)
    {
        if (data.contains(i))
        {
            p->addServoPosition(i,data.value(i));
        }
    }

    QVERIFY(p->toServoSerialData() == output);

}

void tst_Position::cleanup()
{
    delete p;
}

void tst_Position::cleanupTestCase()
{
    delete p;
}
