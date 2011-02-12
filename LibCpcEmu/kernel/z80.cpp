#include "z80.h"

#include <QDebug>

#include "ioport.h"
#include "registerset.h"
#include "z80_macros.h"

static const byte_t PREFIX_CB = 0xcb;
static const byte_t PREFIX_ED = 0xed;

enum OpCodes
{
    nop, ld_bc_word
};

enum OpCodesCB
{
};

enum OpCodesED
{
    out_c_c = 0x49
};


Z80::Z80(Memory* memory)
    : MemoryAccessDevice(memory)
    , m_opCode(0x00)
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
        case nop:           break;
        case ld_bc_word:    LD16_TO_REG(RegisterSet::BC); break;
        default:
            qCritical() << "[Z80] unhandled opcode" << hex << m_opCode;
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
        case out_c_c:       emitOutputRequest(REGISTER_BC, REGISTER_C); break;
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
