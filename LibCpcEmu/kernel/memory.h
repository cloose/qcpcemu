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

    static const RomImageFile* extensionRoms[252];

    static void allocateMemory(quint32 ramSize);
    static void addRomImage(unsigned int number, const RomImageFile *romImage);
};

#endif // MEMORY_H
