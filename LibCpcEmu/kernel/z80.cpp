#include "z80.h"

#include <QDebug>

#include "ioport.h"
#include "registerset.h"
#include "z80_macros.h"

static const byte_t PREFIX_CB = 0xcb;
static const byte_t PREFIX_ED = 0xed;

enum OpCodes
{
    nop, ld_bc_word, inc_bc = 0x03, ld_b_byte = 0x06, ld_de_word = 0x11, inc_de = 0x13,
    jp_nn = 0xc3, di = 0xf3
};

enum OpCodesCB
{
};

enum OpCodesED
{
    out_c_c = 0x49, in_a_c = 0x78
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
        case inc_bc:        REGISTER_BC++; break;
        case ld_b_byte:     LD8_TO_REG(REGISTER_B); break;
        case ld_de_word:    LD16_TO_REG(RegisterSet::DE); break;
        case inc_de:        REGISTER_DE++; break;
        case jp_nn:         JUMP; break;
        case di:            RegisterSet::IFF1 = RegisterSet::IFF2 = 0; m_eiDelay = 0; break;

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
        case in_a_c:        REGISTER_A = emitInputRequest(REGISTER_BC);
            // TODO: FLAGS!!
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
