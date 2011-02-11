#include "gatearray.h"

GateArray::GateArray()
{
}

bool GateArray::in(word_t address, byte_t& value)
{
    Q_UNUSED(address)
    Q_UNUSED(value)

    return false;
}

bool GateArray::out(word_t address, byte_t value)
{
    Q_UNUSED(address)
    Q_UNUSED(value)

    return false;
}
