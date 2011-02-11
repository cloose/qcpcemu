#ifndef GATEARRAY_H
#define GATEARRAY_H

#include "ioport.h"

/**
 * \brief A class representing the gate array in the Amstrad CPC
 */
class GateArray : public IoPort
{
public:
    GateArray();

    virtual bool in(word_t address, byte_t& value);
    virtual bool out(word_t address, byte_t value);
};

#endif // GATEARRAY_H
