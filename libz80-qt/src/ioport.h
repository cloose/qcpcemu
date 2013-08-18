#ifndef IOPORT_H
#define IOPORT_H

#include "types.h"


class IoPort
{
public:
    virtual ~IoPort() {}

    virtual bool in(word_t address, byte_t& value) = 0;
    virtual bool out(word_t address, byte_t value) = 0;
};


#endif // IOPORT_H
