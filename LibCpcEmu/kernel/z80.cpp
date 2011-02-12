#include "z80.h"

Z80::Z80(Memory* memory)
    : MemoryAccessDevice(memory)
{
}

void Z80::registerIoPort(IoPort* port)
{
    m_ioPorts.append(port);
}
