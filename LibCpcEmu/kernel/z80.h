#ifndef Z80_H
#define Z80_H

#include "memoryaccessdevice.h"

struct Memory;


class Z80 : public MemoryAccessDevice
{
public:
    explicit Z80(Memory* memory);
};

#endif // Z80_H
