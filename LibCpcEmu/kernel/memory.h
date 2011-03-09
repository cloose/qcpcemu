#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"


struct Memory
{
    static const byte_t* blocks[4];

    static byte_t*       ram;
    static const byte_t* kernelRom;
    static const byte_t* basicRom;
};

#endif // MEMORY_H
