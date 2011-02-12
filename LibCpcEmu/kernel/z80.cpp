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
    REGISTER_PC = 0x0000;
}

void Z80::step()
{
    m_opCode = fetchInstruction();
}

byte_t Z80::fetchInstruction()
{
    return readByteFromMemory(REGISTER_PC);
}
