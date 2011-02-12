#include "z80.h"

#include "registerset.h"


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
}

byte_t Z80::fetchInstruction()
{
    return readByteFromMemory(REGISTER_PC);
}
