#ifndef Z80_CPU_H
#define Z80_CPU_H

#include "iinputoutput.h"
#include <QtCore/qlist.h>

class IInputOutput;
class IMemory;
class IoPort;

namespace Z80 {

class Alu;

class Cpu : public IInputOutput
{
public:
    explicit Cpu(IMemory *memory);
    virtual ~Cpu();

    void registerIoPort(IoPort* port);
    byte_t emitInputRequest(word_t address);
    void emitOutputRequest(word_t address, byte_t value);

    unsigned execute();
    unsigned executeTStates(unsigned tstates);

    void reset();

    void requestInterrupt(byte_t value = 0);
    void requestNMI();

private:
    Alu *alu;
//    bool nmiRequested;
//    bool interruptRequested;
//    byte_t interruptVector;
    QList<IoPort*> ioPorts;
};

} // namespace Z80

#endif // Z80_CPU_H
