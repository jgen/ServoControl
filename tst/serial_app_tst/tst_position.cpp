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
    //p = new Position();
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
void tst_Position::cleanup()
{
    delete p;
}

void tst_Position::cleanupTestCase()
{
    delete p;
}
