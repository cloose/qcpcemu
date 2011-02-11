#ifndef MEMORYACCESSDEVICE_H
#define MEMORYACCESSDEVICE_H

#include "types.h"

struct Memory;


class MemoryAccessDevice
{
public:
    explicit MemoryAccessDevice(Memory* memory);
    virtual ~MemoryAccessDevice();

    byte_t readByteFromMemory(word_t address);
    word_t readWordFromMemory(word_t address);

private:
    Memory* m_memory;
};

#endif // MEMORYACCESSDEVICE_H
