#ifndef IMEMORY_H
#define IMEMORY_H

#include "types.h"

class IMemory
{
public:
    virtual byte_t read8(word_t address) = 0;
    virtual word_t read16(word_t address) = 0;

    virtual void write8(word_t address, byte_t value) = 0;
    virtual void write16(word_t address, word_t value) = 0;
};

#endif // IMEMORY_H
