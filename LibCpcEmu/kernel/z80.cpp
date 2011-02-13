#include "z80.h"

#include <QDebug>

#include "ioport.h"
#include "memory.h"
#include "registerset.h"
#include "z80_macros.h"
#include "z80_tables.h"


static const byte_t PREFIX_CB = 0xcb;
static const byte_t PREFIX_DD = 0xdd;
static const byte_t PREFIX_ED = 0xed;
static const byte_t PREFIX_FD = 0xfd;


Z80::Z80(Memory* memory)
    : MemoryAccessDevice(memory)
    , m_opCode(0x00)
    , m_eiDelay(0)
{
}

void Z80::registerIoPort(IoPort* port)
{
    m_ioPorts.append(port);
}

void Z80::reset()
{
    // main registers
    REGISTER_AF = 0x0000;
    REGISTER_BC = 0x0000;
    REGISTER_DE = 0x0000;
    REGISTER_HL = 0x0000;
    REGISTER_PC = 0x0000;
    REGISTER_SP = 0xc000;

    // shadow registers
    REGISTER_AF1 = 0x0000;
    REGISTER_BC1 = 0x0000;
    REGISTER_DE1 = 0x0000;
    REGISTER_HL1 = 0x0000;

    // disable interrupts on reset (Z80 manual  p.23)
    RegisterSet::IFF1 = RegisterSet::IFF2 = 0;
}

void Z80::step()
{
    m_opCode = fetchInstruction();

    switch (m_opCode)
    {
        case PREFIX_CB:
            m_opCode = fetchInstruction();
            executeOpCodeCB();
            break;

        case PREFIX_ED:
            m_opCode = fetchInstruction();
            executeOpCodeED();
            break;

        default:
            executeOpCode();
            break;
    }
}

byte_t Z80::fetchInstruction()
{
    return readByteFromMemory(REGISTER_PC++);
}

void Z80::executeOpCode()
{
    switch (m_opCode)
    {
        case 0x00: /* nop */        break;

        case 0x01: /* ld bc,nn */   LD16_TO_REG(RegisterSet::BC); break;
        case 0x02: /* ld (bc),a */  m_memory->ram[REGISTER_BC] = REGISTER_A; break;
        case 0x03: /* inc bc */     REGISTER_BC++; break;
        case 0x04: /* inc b */      Inc(REGISTER_B); break;
        case 0x05: /* dec b */      Dec(REGISTER_B); break;
        case 0x06: /* ld b,n */     LD8_TO_REG(REGISTER_B); break;
        case 0x0a: /* ld a,(bc) */  REGISTER_A = readByteFromMemory(REGISTER_BC); break;
        case 0x0b: /* dec bc */     REGISTER_BC--; break;
        case 0x0c: /* inc c */      Inc(REGISTER_C); break;
        case 0x0d: /* dec c */      Dec(REGISTER_C); break;
        case 0x0e: /* ld c,n */     LD8_TO_REG(REGISTER_C); break;

        case 0x11: /* ld de,nn */   LD16_TO_REG(RegisterSet::DE); break;
        case 0x12: /* ld (de),a */  m_memory->ram[REGISTER_DE] = REGISTER_A; break;
        case 0x13: /* inc de */     REGISTER_DE++; break;
        case 0x14: /* inc d */      Inc(REGISTER_D); break;
        case 0x15: /* dec d */      Dec(REGISTER_D); break;
        case 0x16: /* ld d,n */     LD8_TO_REG(REGISTER_D); break;
        case 0x1a: /* ld a,(de) */  REGISTER_A = readByteFromMemory(REGISTER_DE); break;
        case 0x1b: /* dec de */     REGISTER_DE--; break;
        case 0x1c: /* inc e */      Inc(REGISTER_E); break;
        case 0x1d: /* dec e */      Dec(REGISTER_E); break;
        case 0x1e: /* ld e,n */     LD8_TO_REG(REGISTER_E); break;

        case 0x21: /* ld hl,nn */   LD16_TO_REG(RegisterSet::HL); break;
        case 0x23: /* inc hl */     REGISTER_HL++; break;
        case 0x24: /* inc h */      Inc(REGISTER_H); break;
        case 0x25: /* dec h */      Dec(REGISTER_H); break;
        case 0x26: /* ld h,n */     LD8_TO_REG(REGISTER_H); break;
        case 0x2b: /* dec hl */     REGISTER_HL--; break;
        case 0x2c: /* inc l */      Inc(REGISTER_L); break;
        case 0x2d: /* dec l */      Dec(REGISTER_L); break;
        case 0x2e: /* ld l,n */     LD8_TO_REG(REGISTER_L); break;

        case 0x31: /* ld sp,nn */   LD16_TO_REG(RegisterSet::SP); break;
        case 0x33: /* inc sp */     REGISTER_SP++; break;
        case 0x3b: /* dec sp */     REGISTER_SP--; break;
        case 0x3c: /* inc a */      Inc(REGISTER_A); break;
        case 0x3d: /* dec a */      Dec(REGISTER_A); break;
        case 0x3e: /* ld a,n */     LD8_TO_REG(REGISTER_A); break;

        case 0xc3: /* jp nn */      JUMP; break;
        case 0xf3: /* di */         RegisterSet::IFF1 = RegisterSet::IFF2 = 0; m_eiDelay = 0; break;

        default:
            qCritical() << "[Z80] unhandled opcode" << hex << m_opCode << "at PC" << REGISTER_PC-1;
            break;
    }
}

void Z80::executeOpCodeCB()
{
    switch (m_opCode)
    {
        default:
            qCritical() << "[Z80] unhandled opcode 0xcb" << hex << m_opCode;
            break;
    }
}

void Z80::executeOpCodeED()
{
    switch (m_opCode)
    {
        case 0x49: /* out (c),c */  emitOutputRequest(REGISTER_BC, REGISTER_C); break;
        case 0x78: /* in a,(c) */
            REGISTER_A = emitInputRequest(REGISTER_BC);
            REGISTER_F = (REGISTER_F & C_FLAG)
                       | SignAndZeroTable[REGISTER_A]
                       | ParityTable[REGISTER_A];
            break;

        default:
            qCritical() << "[Z80] unhandled opcode 0xed" << hex << m_opCode;
            break;
    }
}

byte_t Z80::emitInputRequest(word_t address)
{
    byte_t result = 0x00;

    foreach (IoPort* port, m_ioPorts)
    {
        if (port->in(address, result))
            break;
    }

    return result;
}

void Z80::emitOutputRequest(word_t address, byte_t value)
{
    foreach (IoPort* port, m_ioPorts)
    {
        if (port->out(address, value))
            break;
    }
}
