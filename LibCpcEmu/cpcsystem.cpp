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

Keyboard* CpcSystem::keyboard() const
{
    return d->keyboard;
}

void CpcSystem::attachDiskDrive(uint number, FloppyDiskDrive* drive)
{
    d->floppyController->attachDiskDrive(number, drive);
}

void CpcSystem::setRenderer(ScreenRenderer* renderer)
{
    d->gateArray->setRenderer(renderer);
}

void CpcSystem::loadExternalRom(quint8 romNumber, const QString& fileName)
{
    d->loadExternalRom(romNumber, fileName);
}

void CpcSystem::addBreakpoint(word_t address)
{
    d->breakpoints.append(address);
}

void CpcSystem::stopSystem()
{
    d->done = true;
}
