#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"


struct Memory
{
    const byte_t* blocks[4];

    byte_t*       ram;
    const byte_t* kernelRom;
    const byte_t* basicRom;
};

#endif // MEMORY_H
