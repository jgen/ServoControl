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

void tst_Sequence::testAddStartPositionInvalid_data()
{
    QTest::addColumn<Position*>("p");
    Position* sweep = new Position();
    sweep->fromString("*PWMRep,025,PWMSweep,004,001,097,002,081,003,049,004,060,005,071,006,076,007,090,008,094,009,097,010,076,011,091,012,078");
    Position* delay = new Position();
    delay->fromString("*001,097,002,081,003,049,004,060,005,071,006,076,007,090,008,094,009,097,010,076,011,091,012,078,SeqDelay,003");
    QTest::newRow("sweep") << sweep;//don't allow start positions to have a sweep or a delay
    QTest::newRow("delay") << delay;
}
void tst_Sequence::testAddStartPositionInvalid()
{
    QFETCH(Position*,p);
    QVERIFY(!s->setStartPosition(p));
}
void tst_Sequence::testfromStringValid_data()
{
    QTest::addColumn<QString>("first");
    QTest::addColumn<QString>("second");//To ensure overwrite.

    QString first = "*001,050\n*001,066\n*001,054\n*001,066";
    QString second = "*001,060\n*001,060\n*001,074\n*001,086";

    QTest::newRow("Normal Rows") << first << second;

    first = "*001,050";
    second = "*001,060";

    QTest::newRow("One Position") << first << second;

    first = "*001,050\n\n*001,066\n*001,054\n#This is a comment\n*001,066";
    second = "*001,060\n*001,060\n&001,074\n*001,086";

    QTest::newRow("Comments Included") << first << second;
}

void tst_Sequence::testfromStringValid()
{
    QFETCH(QString,first);
    QFETCH(QString,second);

    QVERIFY(s->fromString(first));
    QVERIFY(s->fromString(second));

    QVERIFY(s->isVaild(first));
    QVERIFY(s->isVaild(second));

    Sequence r;
    r.fromString(first);
    QVERIFY(r.toString() != s->toString());

    QVERIFY(!s->isEmpty());
    s->clearStoredPositions();
    QVERIFY(!r.isEmpty());
    r.clearStoredPositions();
    QVERIFY(s->isEmpty());
    QVERIFY(r.isEmpty());

}

void tst_Sequence::testFromStringInvalid_data()
{
    QTest::addColumn<QString>("badString");

    QString string = "";
    QTest::newRow("Empty String") << string;

    string = "001,050\n";
    QTest::newRow("Missing Start Character") << string;

    string = "*001,150\n";
    QTest::newRow("Missing Start Character") << string;

}

void tst_Sequence::testFromStringInvalid()
{
    QFETCH(QString,badString);
    QVERIFY(!s->fromString(badString));
    QVERIFY(!s->isVaild(badString));

}

void tst_Sequence::testIteration()
{
    QVERIFY(s->fromString("*001,050,SeqDelay,002\n*001,066\n*001,054\n"));
    QVERIFY(s->setDelay(4));

    Position* r;
    Position p;
    QByteArray t;

    s->resetIterator();

    QVERIFY(s->hasNext());
    QVERIFY(s->getNextDelay() == 2);
    p.fromString("*001,050,SeqDelay,002");
    t = s->getNextData(r);
    QVERIFY(t.contains(p.toServoSerialData()));

    QVERIFY(s->hasNext());
    QVERIFY(s->getNextDelay() == 4);
    p.fromString("*001,066");
    t = s->getNextData(r);
    QVERIFY(t.contains(p.toServoSerialData()));

    QVERIFY(s->hasNext());
    QVERIFY(s->getNextDelay() == 4);
    p.fromString("*001,054");
    t = s->getNextData(r);
    QVERIFY(t.contains(p.toServoSerialData()));

    QVERIFY(!s->hasNext());
    QVERIFY(!s->getNextDelay());
    QVERIFY(s->getNextData(r).size() == 0);

    s->resetIterator();
    QVERIFY(s->hasNext());

}

void tst_Sequence::testSetDelay()
{
    QVERIFY(!s->setDelay(30));
    QVERIFY(s->setDelay(10));
    QVERIFY(!s->setDelay(0));
}

void tst_Sequence::testReplay()
{
    QVERIFY(!s->setReplay(26));
    QVERIFY(s->setReplay(1));
    QVERIFY(s->getRepeats() == 1);
    QVERIFY(s->setReplay(2));
    QVERIFY(s->getRepeats() == 2);
    QVERIFY(s->setReplay(10));
    QVERIFY(s->getRepeats() == 10);
    QVERIFY(s->setReplay(25));
    QVERIFY(s->getRepeats() == 25);
    QVERIFY(s->setReplay(50));
    QVERIFY(s->getRepeats() == 50);
    QVERIFY(s->setReplay(100));
    QVERIFY(s->getRepeats() == 100);
    QVERIFY(s->setReplay(150));
    QVERIFY(s->getRepeats() == 150);
    QVERIFY(s->setReplay(200));
    QVERIFY(s->getRepeats() == 200);
}

void tst_Sequence::testsetPWMValues()
{
    QVERIFY(!s->setPWMValues(9,0));
    QVERIFY(!s->setPWMValues(2,16));
    QVERIFY(s->setPWMValues(5,14));
}

void tst_Sequence::testSetStartPosition()
{
    Position* p = new Position();
    QVERIFY(s->setStartPosition(p));
    QVERIFY(s->hasStartPosition());
    QVERIFY(s->getStartPosition() == p);
    p->fromString("*PWMRep,010,PWMSweep,003,009,049,010,049,011,049,012,049");
    QVERIFY(!s->setStartPosition(p));
    p->fromString("*009,049,010,049,011,049,012,049,SeqDelay,002");
    QVERIFY(!s->setStartPosition(p));
}

void tst_Sequence::testIsEmpty()
{
    QVERIFY(s->isEmpty());
}

void tst_Sequence::testparseFileHeaderValid_data()
{
    QTest::addColumn<QString>("TestString");

    QTest::newRow("Working") << "000,000,001,002,002,004";
    QTest::newRow("HighRunFormat") << "004,000,001,002,002,004";
    QTest::newRow("HighDatabank") << "000,005,001,002,002,004";
    QTest::newRow("HighPWMRepeat") << "000,000,007,002,002,004";
    QTest::newRow("HighPWMSweep") << "000,000,001,015,002,004";
    QTest::newRow("HighSequenceDelay") << "000,000,001,002,015,004";
    QTest::newRow("HighSequenceReplay") << "000,000,001,002,002,007";

}

void tst_Sequence::testparseFileHeaderValid()
{
    QFETCH(QString,TestString);
    QVERIFY(s->parseFileHeader(TestString));
}

void tst_Sequence::testparseFileHeaderInvalid_data()

{
    QTest::addColumn<QString>("TestString");
    QTest::newRow("runFormatTooHigh") << "005,000,001,002,002,004";
    QTest::newRow("runFormatNotANumber") << "0A5,000,001,002,002,004";
    QTest::newRow("TooHighDatabank") << "000,006,001,002,002,004";
    QTest::newRow("TooHighPWMRepeat") << "000,000,008,002,002,004";
    QTest::newRow("TooHighPWMSweep") << "000,000,001,016,002,004";
    QTest::newRow("TooHighSequenceDelay") << "000,000,001,002,016,004";
    QTest::newRow("TooHighSequenceReplay") << "000,000,001,002,002,008";
}
void tst_Sequence::testparseFileHeaderInvalid()
{
    QFETCH(QString,TestString);
    QVERIFY(!s->parseFileHeader(TestString));
}

void tst_Sequence::testheaderToString_data()
{
    QTest::addColumn<QString>("TestString");

    QTest::newRow("Working") << "000,000,001,002,002,004";
    QTest::newRow("HighRunFormat") << "004,000,001,002,002,004";
    QTest::newRow("HighDatabank") << "000,005,001,002,002,004";
    QTest::newRow("HighPWMRepeat") << "000,000,007,002,002,004";
    QTest::newRow("HighPWMSweep") << "000,000,001,015,002,004";
    QTest::newRow("HighSequenceDelay") << "000,000,001,002,015,004";
    QTest::newRow("HighSequenceReplay") << "000,000,001,002,002,007";
}
void tst_Sequence::testheaderToString()
{
    QFETCH(QString,TestString);
    QVERIFY(s->parseFileHeader(TestString));
    Sequence r;
    QString t = s->headerToString();
    r.parseFileHeader(t);
    QVERIFY(r.headerToString() == s->headerToString());
}



