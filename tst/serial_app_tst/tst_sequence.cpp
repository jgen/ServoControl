#include "tst_sequence.h"

tst_Sequence::tst_Sequence(QObject *parent) :
    QObject(parent)
{
}

/*Work around to suppress qDebug statments from tested units*/

void seqMessageOutput(QtMsgType type, const char *msg)
{
}
void tst_Sequence::initTestCase()
{
    qInstallMsgHandler(seqMessageOutput);
}

void tst_Sequence::init()
{
    s = new Sequence(this);

}
void tst_Sequence::cleanup()
{
    delete s;
}
void tst_Sequence::cleanupTestCase()
{

}

void tst_Sequence::addPositions()
{
    Position* p = new Position();
    p->fromString("*001,049,002,049,003,049,004,049,005,049,006,049,007,049,008,049,009,049,010,049,011,049,012,049");
    s->addPosition(p);
    p = new Position();
    p->fromString("&PWMRep,025,PWMSweep,004,001,097,002,081,003,049,004,060,005,071,006,076,007,090,008,094,009,097,010,049,011,049,012,049,SeqDelay,006");
    s->addPosition(p);
    p = new Position();
    p->fromString("&PWMRep,025,PWMSweep,004,001,097,002,081,003,049,004,060,005,071,006,076,007,090,008,094,009,097,010,076,011,091,012,078,SeqDelay,003");
    s->addPosition(p);
    p = new Position();
    p->fromString("&001,002,002,002,003,002,004,001,005,022,006,011,007,002,008,002,009,002,010,025,011,022,012,027,SeqDelay,006");
    s->addPosition(p);
}

void tst_Sequence::testAddStartPosition()
{
    Position* p = new Position();
    p->fromString("*001,049,002,049,003,049,004,049,005,049,006,049,007,049,008,049,009,049,010,049,011,049,012,049");
    QVERIFY(!s->hasStartPosition());
    QVERIFY(s->setStartPosition(p));
    QVERIFY(s->hasStartPosition());
    QByteArray out;
    out.append(QByteArray::fromHex("9F0F9C00"));
    out.append(p->toServoSerialData());
    out.append(QByteArray::fromHex("9C009F0D"));
    QVERIFY(out == s->getStartPositionCommand());
}
