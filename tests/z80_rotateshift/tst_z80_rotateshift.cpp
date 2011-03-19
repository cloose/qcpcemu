#include <QtCore/QString>
#include <QtTest/QtTest>


#include "registerset.h"
#include "z80_p.cpp"

class TestRotateShift : public QObject
{
    Q_OBJECT

private slots:
    void rlc();
    void rl();
    void rrc();
    void rr();
    void sla();
    void sra();
    void srl();     // complete
};


void TestRotateShift::rlc()
{
    // example from Z80 user manual
    REGISTER_D = 0x88;
    Rlc(REGISTER_D);

    QVERIFY(REGISTER_D == 0x11);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) == P_FLAG);   // parity even
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // data from bit 7
}

void TestRotateShift::rl()
{
    // example from Z80 user manual
    REGISTER_D  = 0x8f;
    REGISTER_F &= ~C_FLAG;  // reset carry flag
    Rl(REGISTER_D);

    QVERIFY(REGISTER_D == 0x1e);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) == P_FLAG);   // parity even
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // data from bit 7
}

void TestRotateShift::rrc()
{
    // example from Z80 user manual
    REGISTER_A = 0x31;
    Rrc(REGISTER_A);

    QVERIFY(REGISTER_A == 0x98);
    QVERIFY((REGISTER_F & S_FLAG) == S_FLAG);   // negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) != P_FLAG);   // parity odd
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // data from bit 0
}

void TestRotateShift::rr()
{
    // example from Z80 user manual
    REGISTER_H  = 0xdd;
    REGISTER_F &= ~C_FLAG;  // reset carry flag
    Rr(REGISTER_H);

    QVERIFY(REGISTER_H == 0x6e);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) != P_FLAG);   // parity odd
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // data from bit 0
}

void TestRotateShift::sla()
{
    // example from Z80 user manual
    REGISTER_L = 0xb1;
    Sla(REGISTER_L);

    QVERIFY(REGISTER_L == 0x62);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) != P_FLAG);   // parity odd
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // data from bit 7
}

void TestRotateShift::sra()
{
    // example from Z80 user manual
    REGISTER_H = 0xb8;
    Sra(REGISTER_H);

    QVERIFY(REGISTER_H == 0xdc);
    QVERIFY((REGISTER_F & S_FLAG) == S_FLAG);   // negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) != P_FLAG);   // parity odd
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // data from bit 0
}

void TestRotateShift::srl()
{
    // example from Z80 user manual
    REGISTER_B = 0x8f;
    Srl(REGISTER_B);

    QVERIFY(REGISTER_B == 0x47);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // always reset
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) == P_FLAG);   // parity even
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // data from bit 0

    // test zero flag
    REGISTER_B = 0x01;
    Srl(REGISTER_B);

    QVERIFY(REGISTER_B == 0x00);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // always reset
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) == P_FLAG);   // parity even
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // data from bit 0

    // test parity flag
    REGISTER_B = 0x21;
    Srl(REGISTER_B);

    QVERIFY(REGISTER_B == 0x10);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // always reset
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) != P_FLAG);   // parity odd
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // data from bit 0

    // test carry flag
    REGISTER_B = 0x96;
    Srl(REGISTER_B);

    QVERIFY(REGISTER_B == 0x4b);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // always reset
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) == P_FLAG);   // parity even
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // data from bit 0
}

QTEST_APPLESS_MAIN(TestRotateShift);

#include "tst_z80_rotateshift.moc"
