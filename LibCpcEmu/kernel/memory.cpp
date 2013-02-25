#include "memory.h"

const byte_t* Memory::blocks[4];

byte_t*       Memory::ram;
const byte_t* Memory::kernelRom;
const byte_t* Memory::basicRom;

const RomImageFile* Memory::extensionRoms[252];


void Memory::allocateMemory(quint32 ramSize)
{
    // allocate and initialize RAM
    ram = new byte_t[ramSize]();
}

void Memory::addRomImage(unsigned int number, const RomImageFile *romImage)
{
    Q_ASSERT(number < 252);
    extensionRoms[number] = romImage;
}
