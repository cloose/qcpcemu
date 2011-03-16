#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"
#include <QtCore\qmap.h>

class RomImageFile;


struct Memory
{
    static const byte_t* blocks[4];

    static byte_t*       ram;
    static const byte_t* kernelRom;
    static const byte_t* basicRom;

    static QMap<quint8, RomImageFile*> externalRoms;
};

#endif // MEMORY_H
