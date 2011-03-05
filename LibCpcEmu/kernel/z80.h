#ifndef Z80_H
#define Z80_H

#include "types.h"
#include <QtCore/qlist.h>

class IoPort;


class Z80
{
public:
    Z80();

    void registerIoPort(IoPort* port);

    void reset();
    int step();

    // TODO: Better name?
    void checkInterrupt();
    void setInterruptPending();

private:
    byte_t fetchInstruction();
    void waitStates();
    void interruptHandler();

    void executeOpCode();
    void executeOpCodeCB();
    void executeOpCodeED();
    void executeOpCodeXX(word_t& destinationRegister);

    byte_t emitInputRequest(word_t address);
    void emitOutputRequest(word_t address, byte_t value);

    byte_t         m_opCode;
    int            m_cycleCount;
    unsigned int   m_interruptMode;
    unsigned int   m_eiDelay;
    bool           m_interruptPending;
    bool           m_halt;
    QList<IoPort*> m_ioPorts;
};

#endif // Z80_H
