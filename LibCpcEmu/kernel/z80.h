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

private:
    QList<IoPort*> m_ioPorts;
};

#endif // Z80_H
