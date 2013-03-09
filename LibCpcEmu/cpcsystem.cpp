#include "cpcsystem.h"
#include "cpcsystem_p.cpp"

#include <QIODevice>

#include "registerset.h"


CpcSystem::CpcSystem(QObject* parent)
    : QObject(parent)
    , d(new CpcSystemPrivate)
{
    d->done = false;
    // TODO: image name should be variable and depend on the CPC system
    d->setupHardware("cpc664.rom");

}

CpcSystem::~CpcSystem()
{
//    delete d;
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

void CpcSystem::stop()
{
    d->done = true;
}

void CpcSystem::reset()
{
    d->resetHardware();
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

void CpcSystem::setAudioDevice(QIODevice *device)
{
    d->soundGenerator->setAudioDevice(device);
}

void CpcSystem::loadExternalRom(quint8 romNumber, const QString& fileName)
{
    d->loadExternalRom(romNumber, fileName);
}

void CpcSystem::addBreakpoint(word_t address)
{
    d->breakpoints.append(address);
}
