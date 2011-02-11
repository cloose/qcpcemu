#include "memoryaccessdevice.h"

#include "memory.h"


MemoryAccessDevice::MemoryAccessDevice(Memory* memory)
    : m_memory(memory)
{
}

MemoryAccessDevice::~MemoryAccessDevice()
{
}

byte_t MemoryAccessDevice::readByteFromMemory(word_t address)
{
    quint8 block = (address >> 14);
    quint16 addressOffset = block * 0x4000;

    return m_memory->blocks[block][address - addressOffset];
}

word_t MemoryAccessDevice::readWordFromMemory(word_t address)
{
    byte_t low = readByteFromMemory(address++);
    byte_t high = readByteFromMemory(address);

    return (low | (high << 8));
}
