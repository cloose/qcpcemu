#ifndef CPCSYSTEM_H
#define CPCSYSTEM_H

#include "LibCpcEmu_global.h"
#include "types.h"

class CpcSystemPrivate;


class LIBCPCEMUSHARED_EXPORT CpcSystem
{
public:
    CpcSystem();
    ~CpcSystem();

    void run();
    void step();

    void addBreakpoint(word_t address);

private:
    CpcSystemPrivate* const d;
};

#endif // CPCSYSTEM_H
