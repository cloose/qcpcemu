#include <QtTest/QtTest>
#include <QtDebug>

#include "registerset.h"
#include "z80_p.cpp"


class Test8BitArithm : public QObject
{
    Q_OBJECT

private slots:
    void add();     // complete
    void adc();     // complete
    void sub();     // complete
    void sbc();
    void _and();
    void _or();     // complete
    void _xor();    // complete
    void cp();
    void inc();     // complete
    void dec();     // complete
};


void Test8BitArithm::add()
{
    // example from Z80 user manual
    REGISTER_A = 0x44;
    REGISTER_C = 0x11;
    Add(REGISTER_C);

    QVERIFY(REGISTER_A == 0x55);
    QVERIFY(REGISTER_C == 0x11);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no carry from bit 3
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no overflow
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // no carry from bit 7

    // example from "Das Maschinensprachebuch zum CPC"
    REGISTER_A = 0x1f;
    REGISTER_H = 0x43;
    Add(REGISTER_H);

    QVERIFY(REGISTER_A == 0x62);
    QVERIFY(REGISTER_H == 0x43);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // carry from bit 3
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no overflow
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // no carry from bit 7

    // example from "Das Maschinensprachebuch zum CPC"
    REGISTER_A = 0xe1;
    REGISTER_D = 0xa2;
    Add(REGISTER_D);

    QVERIFY(REGISTER_A == 0x83);
    QVERIFY(REGISTER_D == 0xa2);
    QVERIFY((REGISTER_F & S_FLAG) == S_FLAG);   // negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no carry from bit 3
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no overflow
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // carry from bit 7

    // test sign and overflow flags
    REGISTER_A = 0x70;
    REGISTER_C = 0x20;
    Add(REGISTER_C);

    QVERIFY(REGISTER_A == 0x90);
    QVERIFY(REGISTER_C == 0x20);
    QVERIFY((REGISTER_F & S_FLAG) == S_FLAG);   // negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no carry from bit 3
    QVERIFY((REGISTER_F & V_FLAG) == V_FLAG);   // overflow
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // no carry from bit 7

    // test zero and carry flags
    REGISTER_A = 0xff;
    REGISTER_C = 0x01;
    Add(REGISTER_C);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY(REGISTER_C == 0x01);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // zero
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // carry from bit 3
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no overflow
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // carry from bit 7
}


void Test8BitArithm::adc()
{
    // example from Z80 user manual
    REGISTER_A  = 0x16;
    REGISTER_H  = 0x10;
    REGISTER_F |= C_FLAG;   // set carry flag
    Adc(REGISTER_H);

    QVERIFY(REGISTER_A == 0x27);
    QVERIFY(REGISTER_H == 0x10);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no carry from bit 3
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no overflow
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // no carry from bit 7

    // example from "Das Maschinensprachebuch zum CPC"
    REGISTER_A = 0x5a;
    REGISTER_H = 0x19;
    REGISTER_F |= C_FLAG;   // set carry flag
    Adc(REGISTER_H);

    QVERIFY(REGISTER_A == 0x74);
    QVERIFY(REGISTER_H == 0x19);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // carry from bit 3
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no overflow
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // no carry from bit 7

    // test sign and overflow flags
    REGISTER_A  = 0x70;
    REGISTER_C  = 0x20;
    REGISTER_F |= C_FLAG;   // set carry flag
    Adc(REGISTER_C);

    QVERIFY(REGISTER_A == 0x91);
    QVERIFY(REGISTER_C == 0x20);
    QVERIFY((REGISTER_F & S_FLAG) == S_FLAG);   // negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no carry from bit 3
    QVERIFY((REGISTER_F & V_FLAG) == V_FLAG);   // overflow
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // no carry from bit 7

    // test zero and carry flags
    REGISTER_A  = 0xff;
    REGISTER_C  = 0x00;
    REGISTER_F |= C_FLAG;   // set carry flag
    Adc(REGISTER_C);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY(REGISTER_C == 0x00);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // zero
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // carry from bit 3
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no overflow
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // carry from bit 7
}


void Test8BitArithm::sub()
{
    // example from Z80 user manual
    REGISTER_A = 0x29;
    REGISTER_D = 0x11;
    Sub(REGISTER_D);

    QVERIFY(REGISTER_A == 0x18);
    QVERIFY(REGISTER_D == 0x11);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no borrow from bit 4
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no overflow
    QVERIFY((REGISTER_F & N_FLAG) == N_FLAG);   // always set
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // no borrow

    // example from "Das Maschinensprachebuch zum CPC"
    REGISTER_A = 0x36;
    REGISTER_H = 0x15;
    Sub(REGISTER_H);

    QVERIFY(REGISTER_A == 0x21);
    QVERIFY(REGISTER_H == 0x15);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no borrow from bit 4
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no overflow
    QVERIFY((REGISTER_F & N_FLAG) == N_FLAG);   // always set
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // no borrow

    // test sign and carry flags
    REGISTER_A = 0x13;
    REGISTER_C = 0x30;
    Sub(REGISTER_C);

    QVERIFY(REGISTER_A == 0xe3);
    QVERIFY(REGISTER_C == 0x30);
    QVERIFY((REGISTER_F & S_FLAG) == S_FLAG);   // negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no borrow from bit 4
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no overflow
    QVERIFY((REGISTER_F & N_FLAG) == N_FLAG);   // always set
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // borrow

    // test sign and half borrow flags
    REGISTER_A = 0xc4;
    REGISTER_C = 0xaf;
    Sub(REGISTER_C);

    QVERIFY(REGISTER_A == 0x15);
    QVERIFY(REGISTER_C == 0xaf);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // borrow from bit 4
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no overflow
    QVERIFY((REGISTER_F & N_FLAG) == N_FLAG);   // always set
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // no borrow

    // test overflow flag
    REGISTER_A = 0x4a;
    REGISTER_C = 0xbd;
    Sub(REGISTER_C);

    QVERIFY(REGISTER_A == 0x8d);
    QVERIFY(REGISTER_C == 0xbd);
    QVERIFY((REGISTER_F & S_FLAG) == S_FLAG);   // negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // borrow from bit 4
    QVERIFY((REGISTER_F & V_FLAG) == V_FLAG);   // overflow
    QVERIFY((REGISTER_F & N_FLAG) == N_FLAG);   // always set
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // borrow

    // test zero flag
    REGISTER_A = 0x50;
    REGISTER_C = 0x50;
    Sub(REGISTER_C);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY(REGISTER_C == 0x50);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no borrow from bit 4
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no overflow
    QVERIFY((REGISTER_F & N_FLAG) == N_FLAG);   // always set
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // no borrow
}


void Test8BitArithm::sbc()
{
    // example from Z80 user manual
    REGISTER_A  = 0x16;
    REGISTER_H  = 0x05;
    REGISTER_F |= C_FLAG;   // set carry flag
    Sbc(REGISTER_H);

    QVERIFY(REGISTER_A == 0x10);
    QVERIFY(REGISTER_H == 0x05);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no borrow from bit 4
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no overflow
    QVERIFY((REGISTER_F & N_FLAG) == N_FLAG);   // always set
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // no borrow
}


void Test8BitArithm::_and()
{
    // example from Z80 user manual
    REGISTER_A = 0xc3;
    REGISTER_B = 0x7b;
    And(REGISTER_B);

    QVERIFY(REGISTER_A == 0x43);
    QVERIFY(REGISTER_B == 0x7b);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // always set
    QVERIFY((REGISTER_F & P_FLAG) != P_FLAG);   // parity odd
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // always reset
}


void Test8BitArithm::_or()
{
    // example from Z80 user manual
    REGISTER_A = 0x12;
    REGISTER_H = 0x48;
    Or(REGISTER_H);

    QVERIFY(REGISTER_A == 0x5a);
    QVERIFY(REGISTER_H == 0x48);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) == P_FLAG);   // parity even
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // always reset

    // test parity flag
    REGISTER_A = 0x23;
    REGISTER_H = 0x19;
    Or(REGISTER_H);

    QVERIFY(REGISTER_A == 0x3b);
    QVERIFY(REGISTER_H == 0x19);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) != P_FLAG);   // parity odd
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // always reset

    // test sign flag
    REGISTER_A = 0x5c;
    REGISTER_H = 0x89;
    Or(REGISTER_H);

    QVERIFY(REGISTER_A == 0xdd);
    QVERIFY(REGISTER_H == 0x89);
    QVERIFY((REGISTER_F & S_FLAG) == S_FLAG);   // negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) == P_FLAG);   // parity even
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // always reset

    // test zero flag
    REGISTER_A = 0x00;
    REGISTER_H = 0x00;
    Or(REGISTER_H);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY(REGISTER_H == 0x00);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) == P_FLAG);   // parity even
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // always reset
}


void Test8BitArithm::_xor()
{
    // example from Z80 user manual
    REGISTER_A = 0x96;
    REGISTER_C = 0x5d;
    Xor(REGISTER_C);

    QVERIFY(REGISTER_A == 0xcb);
    QVERIFY(REGISTER_C == 0x5d);
    QVERIFY((REGISTER_F & S_FLAG) == S_FLAG);   // negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) != P_FLAG);   // parity odd
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // always reset

    // test sign and parity flags
    REGISTER_A = 0x91;
    REGISTER_C = 0xc4;
    Xor(REGISTER_C);

    QVERIFY(REGISTER_A == 0x55);
    QVERIFY(REGISTER_C == 0xc4);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) == P_FLAG);   // parity even
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // always reset

    // test zero flag
    REGISTER_A = 0x11;
    REGISTER_C = 0x11;
    Xor(REGISTER_C);

    QVERIFY(REGISTER_A == 0x00);
    QVERIFY(REGISTER_C == 0x11);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // always reset
    QVERIFY((REGISTER_F & P_FLAG) == P_FLAG);   // parity even
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // always reset
}


void Test8BitArithm::cp()
{
    // example from Z80 user manual
    REGISTER_A = 0x63;
    REGISTER_B = 0x60;
    Cp(REGISTER_B);

    QVERIFY(REGISTER_A == 0x63);
    QVERIFY(REGISTER_B == 0x60);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no borrow from bit 4
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no overflow
    QVERIFY((REGISTER_F & N_FLAG) == N_FLAG);   // always set
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // no borrow

    // example from "Das Maschinensprachebuch zum CPC"
    REGISTER_A = 0x35;
    REGISTER_B = 0x21;
    Cp(REGISTER_B);

    QVERIFY(REGISTER_A == 0x35);
    QVERIFY(REGISTER_B == 0x21);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no borrow from bit 4
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no overflow
    QVERIFY((REGISTER_F & N_FLAG) == N_FLAG);   // always set
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // no borrow

    // example from "Das Maschinensprachebuch zum CPC"
    REGISTER_A = 0x35;
    REGISTER_B = 0x81;
    Cp(REGISTER_B);

    QVERIFY(REGISTER_A == 0x35);
    QVERIFY(REGISTER_B == 0x81);
    QVERIFY((REGISTER_F & S_FLAG) == S_FLAG);   // negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no borrow from bit 4
    QVERIFY((REGISTER_F & V_FLAG) == V_FLAG);   // overflow
    QVERIFY((REGISTER_F & N_FLAG) == N_FLAG);   // always set
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // borrow
}


void Test8BitArithm::inc()
{
    REGISTER_F &= ~C_FLAG;  // reset carry flag

    // example from Z80 user manual
    REGISTER_D = 0x28;
    Inc(REGISTER_D);

    QVERIFY(REGISTER_D == 0x29);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no carry from bit 3
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // not 0x7f before op
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test zero flag
    REGISTER_D = 0xff;
    Inc(REGISTER_D);

    QVERIFY(REGISTER_D == 0x00);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // zero
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // carry from bit 3
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // not 0x7f before op
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test sign and overflow flags
    REGISTER_D = 0x7f;
    Inc(REGISTER_D);

    QVERIFY(REGISTER_D == 0x80);
    QVERIFY((REGISTER_F & S_FLAG) == S_FLAG);   // negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // carry from bit 3
    QVERIFY((REGISTER_F & V_FLAG) == V_FLAG);   // 0x7f before op
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test carry flag
    REGISTER_F |= C_FLAG;   // set carry flag
    REGISTER_D  = 0x28;
    Inc(REGISTER_D);

    QVERIFY(REGISTER_D == 0x29);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no carry from bit 3
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // not 0x7f before op
    QVERIFY((REGISTER_F & N_FLAG) != N_FLAG);   // always reset
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // not affected
}


void Test8BitArithm::dec()
{
    REGISTER_F &= ~C_FLAG;  // reset carry flag

    // example from Z80 user manual
    REGISTER_D = 0x2a;
    Dec(REGISTER_D);

    QVERIFY(REGISTER_D == 0x29);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no borrow from bit 4
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // not 0x80 before op
    QVERIFY((REGISTER_F & N_FLAG) == N_FLAG);   // always set
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test zero flag
    REGISTER_D = 0x01;
    Dec(REGISTER_D);

    QVERIFY(REGISTER_D == 0x00);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) == Z_FLAG);   // zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no borrow from bit 4
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // not 0x80 before op
    QVERIFY((REGISTER_F & N_FLAG) == N_FLAG);   // always set
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test overflow flag
    REGISTER_D = 0x80;
    Dec(REGISTER_D);

    QVERIFY(REGISTER_D == 0x7f);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) == H_FLAG);   // borrow from bit 4
    QVERIFY((REGISTER_F & V_FLAG) == V_FLAG);   // 0x80 before op
    QVERIFY((REGISTER_F & N_FLAG) == N_FLAG);   // always set
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test sign flag
    REGISTER_D = 0x81;
    Dec(REGISTER_D);

    QVERIFY(REGISTER_D == 0x80);
    QVERIFY((REGISTER_F & S_FLAG) == S_FLAG);   // negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no borrow from bit 4
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no 0x80 before op
    QVERIFY((REGISTER_F & N_FLAG) == N_FLAG);   // always set
    QVERIFY((REGISTER_F & C_FLAG) != C_FLAG);   // not affected

    // test carry flag
    REGISTER_F |= C_FLAG;   // set carry flag
    REGISTER_D  = 0x2a;
    Dec(REGISTER_D);

    QVERIFY(REGISTER_D == 0x29);
    QVERIFY((REGISTER_F & S_FLAG) != S_FLAG);   // not negative
    QVERIFY((REGISTER_F & Z_FLAG) != Z_FLAG);   // not zero
    QVERIFY((REGISTER_F & H_FLAG) != H_FLAG);   // no borrow from bit 4
    QVERIFY((REGISTER_F & V_FLAG) != V_FLAG);   // no 0x80 before op
    QVERIFY((REGISTER_F & N_FLAG) == N_FLAG);   // always set
    QVERIFY((REGISTER_F & C_FLAG) == C_FLAG);   // not affected
}


QTEST_MAIN(Test8BitArithm)
#include "tst_z80_8bit_arithmetic.moc"
