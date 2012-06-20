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
    QTest::addColumn<QByteArray>("outputFreeze");
    QTest::addColumn<QByteArray>("outputBoard");
    QTest::addColumn<int>("boardNumber");
    DataPair forward;
    DataPair middle;
    DataPair backwards;
    Position p,f,b;
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
    f = p;
    f.setFreeze(true);
    b = p;
    b.setBoardNumber(4);
    QTest::newRow("All forward") << forward
                                 << QByteArray::fromHex("90029102920293029402950296029702980299029A029B02")
                                 << QByteArray::fromHex("9F0F90029102920293029402950296029702980299029A029B029F00")
                                 << QByteArray::fromHex("90029102920293029402950296029702980299029A029B02")
                                 <<  1;

    QTest::newRow("All middle") << middle
                                << QByteArray::fromHex("902A912A922A932A942A952A962A972A982A992A9A2A9B2A")
                                << QByteArray::fromHex("9F0F902A912A922A932A942A952A962A972A982A992A9A2A9B2A9F00")
                                << QByteArray::fromHex("A02AA12AA22AA32AA42AA52AA62AA72AA82AA92AAA2AAB2A")
                                << 2;
    QTest::newRow("Backwards") << backwards
                               << QByteArray::fromHex("90609160926093609460956096609760986099609A609B60")
                               << QByteArray::fromHex("9F0F90609160926093609460956096609760986099609A609B609F00")
                               << QByteArray::fromHex("B060B160B260B360B460B560B660B760B860B960BA60BB60")
                               << 3;
    QTest::newRow("mirroring") << mirror << p.toServoSerialData() << f.toServoSerialData() << b.toServoSerialData() << 4;
    QTest::newRow("Sparse Even") << sparseEven << QByteArray::fromHex("912A932A952A972A992A9B2A")
                                    << QByteArray::fromHex("9F0F912A932A952A972A992A9B2A9F00")
                                    <<QByteArray::fromHex("D12AD32AD52AD72AD92ADB2A")
                                    << 5;
    QTest::newRow("Sparse Odd") << sparseOdd << QByteArray::fromHex("902A922A942A962A982A9A2A")
                                << QByteArray::fromHex("9F0F902A922A942A962A982A9A2A9F00")
                                << QByteArray::fromHex("E02AE22AE42AE62AE82AEA2A")
                                << 6;
}

void tst_Position::toServoData()
{
    QFETCH(DataPair, data);
    QFETCH(QByteArray,output);
    QFETCH(QByteArray,outputFreeze);
    QFETCH(QByteArray,outputBoard);
    QFETCH(int,boardNumber);

    for (int i(1); i <= 12; ++i)
    {
        if (data.contains(i))
        {
            p->addServoPosition(i,data.value(i));
        }
    }

    QVERIFY2(p->toServoSerialData() == output,"Non freeze failed");
    p->setFreeze(true);
    QVERIFY2(p->toServoSerialData() == outputFreeze,"Freeze failed");
    p->setFreeze(false);
    p->setBoardNumber(boardNumber);
    QVERIFY2(p->toServoSerialData() == outputBoard, "Boardnum failed");

}
void tst_Position::isEmpty()
{
    QVERIFY(p->isEmpty());
    p->addAdvancedPosition(Position::PWMSweep,1);
    QVERIFY(p->isEmpty());
    p->addAdvancedPosition(Position::PWMRepeat,1);
    QVERIFY(p->isEmpty());
    p->addAdvancedPosition(Position::SeqDelay,1);
    QVERIFY(p->isEmpty());
    p->addServoPosition(1,12);
    QVERIFY(!p->isEmpty());
}

void tst_Position::toPWMServoData_data()
{
    QTest::addColumn<QByteArray>("output");
    QTest::addColumn<quint8>("sweep");
    QTest::addColumn<quint8>("repeatIndex");
    QTest::addColumn<quint8>("repeatValue");

    QTest::newRow("reset") << QByteArray::fromHex("9E00") << (quint8)0 << (quint8)0 << (quint8)0;
    QTest::newRow("data set 1")<< QByteArray::fromHex("9E35") << (quint8)5 << (quint8)3 << (quint8)25;
    QTest::newRow("data set 2") << QByteArray::fromHex("9E7A") << (quint8)10 << (quint8)7 << (quint8)200;

}
void tst_Position::toPWMServoData()
{
    QFETCH(QByteArray,output);
    QFETCH(quint8,sweep);
    QFETCH(quint8,repeatIndex);
    QFETCH(quint8,repeatValue);

    bool okay = false;
    p->getPWMSerialData(&okay);
    QVERIFY(!p->hasPWMData() && !okay);
    p->addAdvancedPosition(Position::PWMSweep,sweep);
    p->getPWMSerialData(&okay);
    QVERIFY(!p->hasPWMData() && !okay);
    p->addAdvancedPosition(Position::PWMRepeat,repeatValue);
    QVERIFY(p->hasPWMData());
    QVERIFY(p->getPWMSerialData(&okay) == output);
    QVERIFY(okay);
    p->addAdvancedPositionIndex(Position::PWMRepeat,repeatIndex);
    okay = false;
    QVERIFY(p->hasPWMData());
    QVERIFY(p->getPWMSerialData(&okay) == output);
    QVERIFY(okay);

}
void tst_Position::hasDataFor_data()
{
    QTest::addColumn<quint8>("address");
    QTest::addColumn<quint8>("data");

    QTest::newRow("data set 1") << (quint8) 1 << (quint8)3;
    QTest::newRow("data set 2") << (quint8) 2 << (quint8)12;
    QTest::newRow("data set 3") << (quint8) 3 << (quint8)21;
    QTest::newRow("data set 4") << (quint8) 4 << (quint8)30;
    QTest::newRow("data set 5") << (quint8) 5 << (quint8)39;
    QTest::newRow("data set 6") << (quint8) 6 << (quint8)48;
    QTest::newRow("data set 7") << (quint8) 7 << (quint8)57;
    QTest::newRow("data set 8") << (quint8) 8 << (quint8)66;
    QTest::newRow("data set 9") << (quint8) 9 << (quint8)76;
    QTest::newRow("data set 10") << (quint8) 10 << (quint8)85;
    QTest::newRow("data set 11") << (quint8) 11 << (quint8)94;
    QTest::newRow("data set 12") << (quint8) 12 << (quint8)2;
}
void tst_Position::hasDataFor()
{
    QFETCH(quint8,address);
    QFETCH(quint8,data);
    QVERIFY(!p->hasPositonDataFor(address));
    p->addServoPosition(address,data);
    QVERIFY(p->hasPositonDataFor(address));
    QVERIFY(p->getPositionDataFor(address) == data);
}
void tst_Position::fromStringValid_data() //Add parsing of the special values
{
    QTest::addColumn<QString>("inputString");
    QTest::addColumn<DataPair>("actualData");

    DataPair forward,middle,backward;

    for (int i(1); i <= 12; ++i)
    {
        forward.insert(i,2);
        middle.insert(i,45);
        backward.insert(i,97);
    }

    QTest::newRow("Forwards") << "001,002,002,002,003,002,004,002,005,002,006,002,007,002,008,002,009,002,010,002,011,002,012,002"
                                 << forward;
    QTest::newRow("middle") << "001,045,002,045,003,045,004,045,005,045,006,045,007,045,008,045,009,045,010,045,011,045,012,045"
                               << middle;
    QTest::newRow("backwards") << "001,097,002,097,003,097,004,097,005,097,006,097,007,097,008,097,009,097,010,097,011,097,012,097"
                                  << backward;


}
void tst_Position::fromStringValid()
{
    QFETCH(QString,inputString);
    QFETCH(DataPair,actualData);

    p->fromString(inputString);
    for (int i(1); i <= 12; ++i)
    {
        if (p->hasPositonDataFor(i))
        {
            QVERIFY(p->getPositionDataFor(i) == actualData.value(i));
        }
    }
}

void tst_Position::cleanup()
{
    delete p;
}

void tst_Position::cleanupTestCase()
{
    delete p;
}
