#ifndef Z80_H
#define Z80_H

#include "memoryaccessdevice.h"
#include <QtCore/qlist.h>

class IoPort;


class Z80 : public MemoryAccessDevice
{
public:
    explicit Z80(Memory* memory);

    void registerIoPort(IoPort* port);

    void reset();
    void step();

private:
    byte_t fetchInstruction();

    void executeOpCode();
    void executeOpCodeCB();
    void executeOpCodeED();

    byte_t emitInputRequest(word_t address);
    void emitOutputRequest(word_t address, byte_t value);

    byte_t         m_opCode;
    unsigned int   m_eiDelay;
    QList<IoPort*> m_ioPorts;
};

#endif // Z80_H
