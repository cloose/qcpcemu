#include <cstring>

#include "gatearray.h"
#include "iocontroller.h"
#include "memory.h"
#include "romimagefile.h"
#include "videocontroller.h"
#include "z80.h"


class CpcSystemPrivate
{
public:
    ~CpcSystemPrivate();

    void setupHardware();

    GateArray* gateArray;
    IoController* ioController;
    VideoController* videoController;
    Z80* cpu;

    QList<word_t> breakpoints;

    RomImageFile* systemRom;
};


CpcSystemPrivate::~CpcSystemPrivate()
{
    delete cpu;
    delete videoController;
    delete ioController;
    delete gateArray;
    delete systemRom;
}


void CpcSystemPrivate::setupHardware()
{
    // load the system ROM image from file
    // TODO: image name should be variable and depend on the CPC system
    systemRom = new RomImageFile("cpc464.rom");
    if (!systemRom->load())
    {
        // TODO: missing error handling and reporting
        return;
    }

    // create and initialize the RAM
    Memory memory;

    memory.ram = new byte_t[64*1024];
    ::memset(memory.ram, 0, 64*1024);

    memory.kernelRom = systemRom->constData();
    memory.basicRom = systemRom->constData() + (16*1024);

    memory.blocks[0] = memory.kernelRom;
    memory.blocks[1] = memory.ram + 0x4000;
    memory.blocks[2] = memory.ram + 0x8000;
    memory.blocks[3] = memory.basicRom;

    ioController = new IoController();

    videoController = new VideoController();

    QObject::connect(videoController, SIGNAL(vSync(bool)),
                     ioController, SLOT(vSync(bool)));

    cpu = new Z80();

    gateArray = new GateArray(cpu, videoController);

    cpu->registerIoPort(gateArray);
    cpu->registerIoPort(ioController);
    cpu->registerIoPort(videoController);
}
