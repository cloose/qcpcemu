#ifndef GATEARRAY_H
#define GATEARRAY_H

#include "ioport.h"

struct Memory;


/**
 * \brief A class representing the gate array in the Amstrad CPC
 */
class GateArray : public IoPort
{
public:
    explicit GateArray(Memory* memory);

    virtual bool in(word_t address, byte_t& value);
    virtual bool out(word_t address, byte_t value);

private:
    void setRomConfiguration(byte_t value);

    Memory* m_memory;
};

#endif // GATEARRAY_H
