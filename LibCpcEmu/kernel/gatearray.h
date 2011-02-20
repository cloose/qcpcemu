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

private:
    void selectPen(byte_t value);
    void selectColorForPen(byte_t value);
    void setRomConfiguration(byte_t value);

    byte_t m_inkValues[17];
    byte_t m_currentPen;
    byte_t m_scanlineCounter;
};

#endif // GATEARRAY_H
