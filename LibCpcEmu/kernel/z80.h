#ifndef Z80_H
#define Z80_H

#include "types.h"
#include <QtCore/qlist.h>

class IoPort;
class VideoController;


class Z80
{
public:
    explicit Z80(VideoController* crtc);

    void registerIoPort(IoPort* port);

    void reset();
    void step();

private:
    byte_t fetchInstruction();
    void waitStates();
    void interruptHandler();

    void executeOpCode();
    void executeOpCodeCB();
    void executeOpCodeED();

    byte_t emitInputRequest(word_t address);
    void emitOutputRequest(word_t address, byte_t value);

    VideoController* m_crtc;

    byte_t         m_opCode;
    int            m_cycleCount;
    int            m_frameCycleCount;
    unsigned int   m_interruptMode;
    unsigned int   m_eiDelay;
    bool           m_interruptPending;
    bool           m_halt;
    QList<IoPort*> m_ioPorts;
};

#endif // Z80_H
