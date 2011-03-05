#include "cpcsystem.h"
#include "cpcsystem_p.cpp"

#include "registerset.h"


CpcSystem::CpcSystem()
    : d(new CpcSystemPrivate)
{
    d->setupHardware();
}

CpcSystem::~CpcSystem()
{
    delete d;
}

void CpcSystem::run()
{
    do
    {
        //d->cpu->step();
        d->gateArray->run();
    }
    while (!d->breakpoints.contains(REGISTER_PC));
}

void CpcSystem::step()
{
//    d->cpu->step();
    d->gateArray->run();
}

void CpcSystem::setRenderer(ScreenRenderer* renderer)
{
    d->gateArray->setRenderer(renderer);
}

void CpcSystem::addBreakpoint(word_t address)
{
    d->breakpoints.append(address);
}
