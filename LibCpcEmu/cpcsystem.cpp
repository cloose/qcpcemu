#include "cpcsystem.h"
#include "cpcsystem_p.cpp"

#include "registerset.h"


CpcSystem::CpcSystem()
    : d(new CpcSystemPrivate)
{
    d->done = false;
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
        d->gateArray->run();
    }
    while (!d->done && !d->breakpoints.contains(REGISTER_PC));
}

void CpcSystem::step()
{
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

void CpcSystem::stopSystem()
{
    d->done = true;
}
