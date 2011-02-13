#include "memory.h"

const byte_t* Memory::blocks[4];

byte_t*       Memory::ram;
const byte_t* Memory::kernelRom;
const byte_t* Memory::basicRom;
