#include <QtTest/QtTest>
#include <QtDebug>

#include "registerset.h"
#include "z80_p.cpp"


class TestBitSet : public QObject
{
    Q_OBJECT

private slots:
    void bit();     // complete
    void set();     // complete
    void res();     // complete
};


void TestBitSet::bit()
{
    REGISTER_F &= ~C_FLAG;  // reset carry flag

    // test bit 0
    REGISTER_A = 0x00;
    Bit(0, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // bit 0 is 0
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 0
    REGISTER_A = 0x01;
    Bit(0, REGISTER_A);

    QVERIFY(REGISTER_A == 0x01);
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // bit 0 is 1
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 1
    REGISTER_A = 0x00;
    Bit(1, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // bit 1 is 0
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 1
    REGISTER_A = 0x02;
    Bit(1, REGISTER_A);

    QVERIFY(REGISTER_A == 0x02);
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // bit 1 is 1
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 2
    REGISTER_A = 0x00;
    Bit(2, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // bit 2 is 0
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 2
    REGISTER_A = 0x04;
    Bit(2, REGISTER_A);

    QVERIFY(REGISTER_A == 0x04);
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // bit 2 is 1
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 3
    REGISTER_A = 0x00;
    Bit(3, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // bit 3 is 0
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 3
    REGISTER_A = 0x08;
    Bit(3, REGISTER_A);

    QVERIFY(REGISTER_A == 0x08);
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // bit 3 is 1
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 4
    REGISTER_A = 0x00;
    Bit(4, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // bit 4 is 0
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 4
    REGISTER_A = 0x10;
    Bit(4, REGISTER_A);

    QVERIFY(REGISTER_A == 0x10);
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // bit 4 is 1
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 5
    REGISTER_A = 0x00;
    Bit(5, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // bit 5 is 0
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 5
    REGISTER_A = 0x20;
    Bit(5, REGISTER_A);

    QVERIFY(REGISTER_A == 0x20);
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // bit 5 is 1
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 6
    REGISTER_A = 0x00;
    Bit(6, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // bit 6 is 0
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 6
    REGISTER_A = 0x40;
    Bit(6, REGISTER_A);

    QVERIFY(REGISTER_A == 0x40);
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // bit 6 is 1
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 7
    REGISTER_A = 0x00;
    Bit(7, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // bit 7 is 0
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 7
    REGISTER_A = 0x80;
    Bit(7, REGISTER_A);

    QVERIFY(REGISTER_A == 0x80);
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // bit 7 is 1
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 3 with A <> 0x00
    REGISTER_A = 0xf7;
    Bit(3, REGISTER_A);

    QVERIFY(REGISTER_A == 0xf7);
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // bit 3 is 0
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test bit 3 with A <> 0x08
    REGISTER_A = 0xff;
    Bit(3, REGISTER_A);

    QVERIFY(REGISTER_A == 0xff);
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // bit 3 is 1
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected
}


void TestBitSet::set()
{
    // test set bit 0
    REGISTER_A = 0x00;
    REGISTER_F = 0xff;
    Set(0, REGISTER_A);

    QVERIFY(REGISTER_A == 0x01);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test set bit 1
    REGISTER_A = 0x00;
    REGISTER_F = 0xff;
    Set(1, REGISTER_A);

    QVERIFY(REGISTER_A == 0x02);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test set bit 2
    REGISTER_A = 0x00;
    REGISTER_F = 0xff;
    Set(2, REGISTER_A);

    QVERIFY(REGISTER_A == 0x04);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test set bit 3
    REGISTER_A = 0x00;
    REGISTER_F = 0xff;
    Set(3, REGISTER_A);

    QVERIFY(REGISTER_A == 0x08);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test set bit 4
    REGISTER_A = 0x00;
    REGISTER_F = 0xff;
    Set(4, REGISTER_A);

    QVERIFY(REGISTER_A == 0x10);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test set bit 5
    REGISTER_A = 0x00;
    REGISTER_F = 0xff;
    Set(5, REGISTER_A);

    QVERIFY(REGISTER_A == 0x20);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test set bit 6
    REGISTER_A = 0x00;
    REGISTER_F = 0xff;
    Set(6, REGISTER_A);

    QVERIFY(REGISTER_A == 0x40);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test set bit 7
    REGISTER_A = 0x00;
    REGISTER_F = 0xff;
    Set(7, REGISTER_A);

    QVERIFY(REGISTER_A == 0x80);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test set bit 3 with A <> 0x00
    REGISTER_A = 0xf7;
    REGISTER_F = 0xff;
    Set(3, REGISTER_A);

    QVERIFY(REGISTER_A == 0xff);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected
}


void TestBitSet::res()
{
    // test reset bit 0
    REGISTER_A = 0x01;
    REGISTER_F = 0xff;
    Res(0, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test reset bit 1
    REGISTER_A = 0x02;
    REGISTER_F = 0xff;
    Res(1, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test reset bit 2
    REGISTER_A = 0x04;
    REGISTER_F = 0xff;
    Res(2, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test reset bit 3
    REGISTER_A = 0x08;
    REGISTER_F = 0xff;
    Res(3, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test reset bit 4
    REGISTER_A = 0x10;
    REGISTER_F = 0xff;
    Res(4, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test reset bit 5
    REGISTER_A = 0x20;
    REGISTER_F = 0xff;
    Res(5, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test reset bit 6
    REGISTER_A = 0x40;
    REGISTER_F = 0xff;
    Res(6, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test reset bit 7
    REGISTER_A = 0x80;
    REGISTER_F = 0xff;
    Res(7, REGISTER_A);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected

    // test reset bit 3 with A <> 0x08
    REGISTER_A = 0xff;
    REGISTER_F = 0xff;
    Res(3, REGISTER_A);

    QVERIFY(REGISTER_A == 0xf7);
    QVERIFY(REGISTER_F == 0xff);    // flags not affected
}


QTEST_APPLESS_MAIN(TestBitSet);

#include "tst_z80_bitset.moc"
