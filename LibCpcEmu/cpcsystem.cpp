#include "cpcsystem.h"
#include "cpcsystem_p.cpp"

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
    d->cpu->step();
}
