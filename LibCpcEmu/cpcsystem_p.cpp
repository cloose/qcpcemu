#include <cstring>

#include "floppycontroller.h"
#include "gatearray.h"
#include "iocontroller.h"
#include "keyboard.h"
#include "memory.h"
#include "romimagefile.h"
#include "soundgenerator.h"
#include "videocontroller.h"
#include "z80.h"


class CpcSystemPrivate
{
public:
    ~CpcSystemPrivate();

    void setupHardware(const QString& romFileName);
    void resetHardware();
    void loadExternalRom(quint8 romNumber, const QString& fileName);

    bool done;

    GateArray* gateArray;
    IoController* ioController;
    Keyboard* keyboard;
    SoundGenerator* soundGenerator;
    VideoController* videoController;
    FloppyController* floppyController;
    Z80* cpu;

    QList<word_t> breakpoints;

    RomImageFile* systemRom;
};


CpcSystemPrivate::~CpcSystemPrivate()
{
    delete gateArray;

    delete cpu;

    delete floppyController;

    delete videoController;

    delete ioController;

    delete soundGenerator;

    delete keyboard;

    Memory memory;
    delete[] memory.ram;
    memory.ram = 0;
    memory.kernelRom = 0;
    memory.basicRom = 0;
    memory.blocks[0] = 0;
    memory.blocks[1] = 0;
    memory.blocks[2] = 0;
    memory.blocks[3] = 0;

    // TODO: remove external ROMs

    delete systemRom;
}


void CpcSystemPrivate::setupHardware(const QString& romFileName)
{
    // load the system ROM image from file
    systemRom = new RomImageFile(romFileName);
    if (!systemRom->load())
    {
        // TODO: missing error handling and reporting
        return;
    }

    // create and initialize the RAM
    Memory memory;

    Memory::allocateMemory(64*1024);

    memory.kernelRom = systemRom->constData();
    memory.basicRom = systemRom->constData() + (16*1024);

    memory.blocks[0] = memory.kernelRom;
    memory.blocks[1] = memory.ram + 0x4000;
    memory.blocks[2] = memory.ram + 0x8000;
    memory.blocks[3] = memory.basicRom;

    keyboard = new Keyboard();
    soundGenerator = new SoundGenerator();

    ioController = new IoController(keyboard, soundGenerator);

    videoController = new VideoController();

    QObject::connect(videoController, SIGNAL(vSync(bool)),
                     ioController, SLOT(vSync(bool)));

    floppyController = new FloppyController();

    cpu = new Z80();

    gateArray = new GateArray(cpu, videoController, soundGenerator);

    cpu->registerIoPort(gateArray);
    cpu->registerIoPort(videoController);
    cpu->registerIoPort(ioController);
    cpu->registerIoPort(floppyController);
}

void CpcSystemPrivate::resetHardware()
{
    cpu->reset();

    // initialize the RAM
    ::memset(Memory::ram, 0, 64*1024);

    Memory::blocks[0] = Memory::kernelRom;
    Memory::blocks[1] = Memory::ram + 0x4000;
    Memory::blocks[2] = Memory::ram + 0x8000;
    Memory::blocks[3] = Memory::basicRom;
}

void CpcSystemPrivate::loadExternalRom(quint8 romNumber, const QString& fileName)
{
    RomImageFile* romImage = new RomImageFile(fileName);
    romImage->load();

    Memory::addRomImage(romNumber, romImage);
}
