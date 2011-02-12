#ifndef IOCONTROLLER_H
#define IOCONTROLLER_H

#include "ioport.h"


/**
 * \brief The 8255 PPI chip in a Amstrad CPC system
 */
class IoController : public IoPort
{
public:
    IoController();

    virtual bool in(word_t address, byte_t& value);
    virtual bool out(word_t address, byte_t value);

private:
    byte_t m_portA;
    byte_t m_portB;
    byte_t m_portC;
    byte_t m_control;
};

#endif // IOCONTROLLER_H
