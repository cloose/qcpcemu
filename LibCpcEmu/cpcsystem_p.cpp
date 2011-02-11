#include <cstring>
#include "memory.h"


class CpcSystemPrivate
{
public:
    ~CpcSystemPrivate();

    void setupHardware();

    Memory memory;
};


CpcSystemPrivate::~CpcSystemPrivate()
{
}


void CpcSystemPrivate::setupHardware()
{
    // create and initialize the RAM
    memory.ram = new byte_t[64*1024];
    ::memset(memory.ram, 0, 64*1024);
}
