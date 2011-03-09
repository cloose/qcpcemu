#ifndef CPCSYSTEM_H
#define CPCSYSTEM_H

#include "LibCpcEmu_global.h"
#include "types.h"

class CpcSystemPrivate;
class ScreenRenderer;

class LIBCPCEMUSHARED_EXPORT CpcSystem
{
public:
    CpcSystem();
    ~CpcSystem();

    void run();
    void step();

    void setRenderer(ScreenRenderer* renderer);

    void addBreakpoint(word_t address);

private:
    CpcSystemPrivate* const d;
};

#endif // CPCSYSTEM_H
