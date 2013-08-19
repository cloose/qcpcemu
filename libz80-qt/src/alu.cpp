/*
 * Copyright 2013 Christian Loose <christian.loose@hamburg.de>
 *
 * --------------------------------------------------------------------
 * libz80 - Z80 emulation library (Version 2.1.0)
 * (C) Gabriel Gambetta (gabriel.gambetta@gmail.com) 2000 - 2012
 * https://github.com/ggambetta/libz80
 * --------------------------------------------------------------------
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "alu.h"
using namespace Z80;

#include <QDebug>

#include "iinputoutput.h"
#include "imemory.h"
#include "opcodes_table.h"
#include "registers.h"

static int parityBit[256] = {
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1
};

/* Increment or decrement R, preserving bit 7 */
#define INCR (Register::R = (Register::R & 0x80) | ((Register::R + 1) & 0x7f))
#define DECR (Register::R = (Register::R & 0x80) | ((Register::R - 1) & 0x7f))


Alu::Alu(IMemory *mem, IInputOutput *io) :
    memory(mem),
    ioHandler(io),
    tstates(0),
    interruptMode(0),
    halted(false),
    deferInterrupt(false),
    nmiRequested(false),
    interruptRequested(false),
    interruptVector(0)
{
    initOpcodeTables();
}

void Alu::reset()
{
    Register::PC = 0x0000;
    BR.F = 0;
    Register::IFF1 = Register::IFF2 = 0;
    Register::R = 0;
    Register::I = 0;

    tstates = 0;
    interruptMode = 0;
    halted = false;
    deferInterrupt = false;
    nmiRequested = false;
    interruptRequested = false;
}

unsigned Alu::execute()
{
    if (nmiRequested) {
        qFatal("Not Implemented Yet: NMI request in Cpu::execute()");
        return 0;
    } else if (interruptRequested && !deferInterrupt && Register::IFF1) {
        interruptRequested = false;
        return interruptHandler();
    } else {
        deferInterrupt = false;
        step();
        return tstates;
    }
}

unsigned Alu::executeTStates(unsigned minTstates)
{
    tstates = 0;

    while (tstates < minTstates)
        execute();

    return tstates;
}

void Alu::requestInterrupt(byte_t value)
{
    interruptRequested = true;
    interruptVector = value;
}

void Alu::requestNMI()
{
    nmiRequested = true;
}

unsigned Alu::interruptHandler()
{
    unhalt();

    Register::IFF1 = Register::IFF2 = 0;

    switch (interruptMode) {
    case 0:
        qFatal("Not Implemented Yet: interrupt mode 0");
        break;
    case 1:
        doPush(Register::PC);
        Register::PC = 0x0038;
        tstates += 7;
        break;
    case 2:
        qFatal("Not Implemented Yet: interrupt mode 2");
        break;
    }

    return tstates;
}

bool Alu::isInterruptDeferred() const
{
    return deferInterrupt;
}

void Alu::setInterruptDeferred(bool deferred)
{
    deferInterrupt = deferred;
}

void Alu::unhalt()
{
    if (halted) {
        halted = false;
        Register::PC++;
    }
}

void Alu::step()
{
    OpcodeTable *current = opcodes_main;
    OpcodeEntry *entries = current->entries;
    OpcodeFunc func;

    byte_t opcode;
    int offset = 0;
    do {
        opcode = memory->read8(Register::PC + offset);
        Register::PC++;
        tstates += 1;

        INCR;
        func = entries[opcode].func;
        if (func != NULL) {
            Register::PC -= offset;
            (this->*func)();
            Register::PC += offset;
            break;
        } else if (entries[opcode].table) {
            current = entries[opcode].table;
            entries = current->entries;
            offset = current->opcodeOffset;
            if (offset > 0)
                DECR;
        } else {
            break;
        }

    } while (1);
}

byte_t Alu::ioRead(word_t address)
{
    return ioHandler->emitInputRequest(address);
}

void Alu::ioWrite(word_t address, byte_t value)
{
    ioHandler->emitOutputRequest(address, value);
}

int Alu::doComplement(byte_t v)
{
    if ((v & 0x80) == 0)
        return v;

    v = ~v;
    v &= 0x7F;
    v++;

    return -v;
}

byte_t Alu::doArithmetic(byte_t value, int withCarry, int isSub)
{
    ushort res; /* To detect carry */

    if (isSub) {
        setFlag(F_N);
        valFlag(F_H, (((BR.A & 0x0F) - (value & 0x0F)) & 0x10) != 0);
        res = BR.A - value;
        if (withCarry && getFlag(F_C))
            res--;
    } else {
        resFlag(F_N);
        valFlag(F_H, (((BR.A & 0x0F) + (value & 0x0F)) & 0x10) != 0);
        res = BR.A + value;
        if (withCarry && getFlag(F_C))
            res++;
    }
    valFlag(F_S, ((res & 0x80) != 0));
    valFlag(F_C, ((res & 0x100) != 0));
    valFlag(F_Z, ((res & 0xff) == 0));
    int minuend_sign = BR.A & 0x80;
    int subtrahend_sign = value & 0x80;
    int result_sign = res & 0x80;
    int overflow;
    if (isSub)
        overflow = minuend_sign != subtrahend_sign && result_sign != minuend_sign;
    else
        overflow = minuend_sign == subtrahend_sign && result_sign != minuend_sign;
    valFlag(F_PV, overflow);
    adjustFlags(res);

    return (byte_t)(res & 0xFF);
}

word_t Alu::doAddWord(word_t a1, word_t a2, int withCarry, int isSub)
{
    if (withCarry && getFlag(F_C))
        a2++;
    int sum = a1;
    if (isSub) {
        sum -= a2;
        valFlag(F_H, ((a1 & 0x0fff) - (a2 & 0x0fff)) & 0x1000);
    } else {
        sum += a2;
        valFlag(F_H, ((a1 & 0x0fff) + (a2 & 0x0fff)) & 0x1000);
    }
    valFlag(F_C, sum & 0x10000);
    if (withCarry || isSub) {
        int minuend_sign = a1 & 0x8000;
        int subtrahend_sign = a2 & 0x8000;
        int result_sign = sum & 0x8000;
        int overflow;
        if (isSub)
            overflow = minuend_sign != subtrahend_sign && result_sign != minuend_sign;
        else
            overflow = minuend_sign == subtrahend_sign && result_sign != minuend_sign;
        valFlag(F_PV, overflow);
        valFlag(F_S, (sum & 0x8000) != 0);
        valFlag(F_Z, sum == 0);
    }
    valFlag(F_N, isSub);
    adjustFlags(sum >> 8);
    return sum;
}

void Alu::doAND(byte_t value)
{
    BR.A &= value;
    adjustLogicFlag(1);
}

void Alu::doOR(byte_t value)
{
    BR.A |= value;
    adjustLogicFlag(0);
}

void Alu::doXOR(byte_t value)
{
    BR.A ^= value;
    adjustLogicFlag(0);
}

void Alu::doBIT(int b, byte_t val)
{
    if (val & (1 << b))
        resFlag(F_Z | F_PV);
    else
        setFlag(F_Z | F_PV);

    setFlag(F_H);
    resFlag(F_N);

    resFlag(F_S);
    if ((b == 7) && !getFlag(F_Z))
        setFlag(F_S);
}

void Alu::doBIT_r(int b, byte_t val)
{
    doBIT(b, val);
    valFlag(F_5, val & F_5);
    valFlag(F_3, val & F_3);
}

void Alu::doBIT_indexed(int b, word_t address)
{
    byte_t val = memory->read8(address);
    doBIT(b, val);
    valFlag(F_5, (address >> 8) & F_5);
    valFlag(F_3, (address >> 8) & F_3);
}

byte_t Alu::doSetRes(int bit, int pos, byte_t val)
{
    if (bit)
        val |= (1 << pos);
    else
        val &= ~(1 << pos);
    return val;
}

byte_t Alu::doIncDec(byte_t val, int isDec)
{
    if (isDec) {
        valFlag(F_PV, (val & 0x80) && !((val - 1) & 0x80));
        val--;
        valFlag(F_H, (val & 0x0F) == 0x0F);
    } else {
        valFlag(F_PV, !(val & 0x80) && ((val + 1) & 0x80));
        val++;
        valFlag(F_H, !(val & 0x0F));
    }

    valFlag(F_S, ((val & 0x80) != 0));
    valFlag(F_Z, (val == 0));
    valFlag(F_N, isDec);

    adjustFlags(val);

    return val;
}

byte_t Alu::doRLC(int adjFlags, byte_t val)
{
    valFlag(F_C, (val & 0x80) != 0);
    val <<= 1;
    val |= (byte_t)getFlag(F_C);

    adjustFlags(val);
    resFlag(F_H | F_N);

    if (adjFlags)
        adjustFlagSZP(val);

    return val;
}

byte_t Alu::doRL(int adjFlags, byte_t val)
{
    int CY = getFlag(F_C);
    valFlag(F_C, (val & 0x80) != 0);
    val <<= 1;
    val |= (byte_t)CY;

    adjustFlags(val);
    resFlag(F_H | F_N);

    if (adjFlags)
        adjustFlagSZP(val);

    return val;
}

byte_t Alu::doRRC(int adjFlags, byte_t val)
{
    valFlag(F_C, (val & 0x01) != 0);
    val >>= 1;
    val |= ((byte_t)getFlag(F_C) << 7);

    adjustFlags(val);
    resFlag(F_H | F_N);

    if (adjFlags)
        adjustFlagSZP(val);

    return val;
}

byte_t Alu::doRR(int adjFlags, byte_t val)
{
    int CY = getFlag(F_C);
    valFlag(F_C, (val & 0x01));
    val >>= 1;
    val |= (CY << 7);

    adjustFlags(val);
    resFlag(F_H | F_N);

    if (adjFlags)
        adjustFlagSZP(val);

    return val;
}

byte_t Alu::doSL(byte_t val, int isArith)
{
    valFlag(F_C, (val & 0x80) != 0);
    val <<= 1;

    if (!isArith)
        val |= 1;

    adjustFlags(val);
    resFlag(F_H | F_N);
    adjustFlagSZP(val);

    return val;
}

byte_t Alu::doSR(byte_t val, int isArith)
{
    int b = val & 0x80;

    valFlag(F_C, (val & 0x01) != 0);
    val >>= 1;

    if (isArith)
        val |= b;

    adjustFlags(val);
    resFlag(F_H | F_N);
    adjustFlagSZP(val);

    return val;
}

void Alu::doPush(ushort val)
{
    WR.SP--;
    WR.SP--;
    memory->write16(WR.SP, val);
}

ushort Alu::doPop()
{
    ushort val;
    val = memory->read16(WR.SP);
    WR.SP++;
    WR.SP++;
    return val;
}

byte_t Alu::doCP_HL()
{
    byte_t val = memory->read8(WR.HL);
    byte_t result = doArithmetic(val, 0, 1);
    adjustFlags(val);
    return result;
}

void Alu::doDAA()
{
    int correction_factor = 0x00;
    int carry = 0;
    if (BR.A > 0x99 || getFlag(F_C)) {
        correction_factor |= 0x60;
        carry = 1;
    }
    if ((BR.A & 0x0f) > 9 || getFlag(F_H))
        correction_factor |= 0x06;
    int a_before = BR.A;
    if (getFlag(F_N))
        BR.A -= correction_factor;
    else
        BR.A += correction_factor;
    valFlag(F_H, (a_before ^ BR.A) & 0x10);
    valFlag(F_C, carry);
    valFlag(F_S, (BR.A & 0x80) != 0);
    valFlag(F_Z, (BR.A == 0));
    valFlag(F_PV, parityBit[BR.A]);
    adjustFlags(BR.A);
}

int Alu::condition(Z80::Condition cond)
{
    if (cond == C_)
        return 1;

    if (cond == C_Z)
        return getFlag(F_Z);

    if (cond == C_NZ)
        return !getFlag(F_Z);

    if (cond == C_C)
        return getFlag(F_C);

    if (cond == C_NC)
        return !getFlag(F_C);

    if (cond == C_M)
        return getFlag(F_S);

    if (cond == C_P)
//        return !getFlag(F_S | F_Z);
        return !getFlag(F_S);

    if (cond == C_PE)
        return getFlag(F_PV);

    /* if (cond == C_PO)*/
    return !getFlag(F_PV);
}

void Alu::setFlag(Z80::Flags flag)
{
    BR.F |= flag;
}

void Alu::resFlag(Z80::Flags flag)
{
    BR.F &= ~flag;
}

void Alu::valFlag(Z80::Flags flag, int val)
{
    if (val)
        setFlag(flag);
    else
        resFlag(flag);
}

int Alu::getFlag(Z80::Flags flag)
{
    return (BR.F & flag) != 0;
}

void Alu::adjustFlags(byte_t val)
{
    valFlag(F_5, (val & F_5) != 0);
    valFlag(F_3, (val & F_3) != 0);
}

void Alu::adjustFlagSZP(byte_t val)
{
    valFlag(F_S, (val & 0x80) != 0);
    valFlag(F_Z, (val == 0));
    valFlag(F_PV, parityBit[val]);
}

/* Adjust flags after AND, OR, XOR */
void Alu::adjustLogicFlag(int flagH)
{
    valFlag(F_S, (BR.A & 0x80) != 0);
    valFlag(F_Z, (BR.A == 0));
    valFlag(F_H, flagH);
    valFlag(F_N, 0);
    valFlag(F_C, 0);
    valFlag(F_PV, parityBit[BR.A]);

    adjustFlags(BR.A);
}
