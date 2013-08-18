#ifndef IINPUTOUTPUT_H
#define IINPUTOUTPUT_H

#include "types.h"

class IInputOutput
{
public:
    virtual byte_t emitInputRequest(word_t address) = 0;
    virtual void emitOutputRequest(word_t address, byte_t value) = 0;
};

#endif // IINPUTOUTPUT_H
