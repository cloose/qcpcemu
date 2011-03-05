#include "gatearray.h"

#include <QDebug>

#include "memory.h"
#include "screenrenderer.h"
#include "videocontroller.h"
#include "z80.h"


GateArray::GateArray(Z80* cpu, VideoController* crtc, QObject* parent)
    : QObject(parent)
    , m_currentPen(0)
    , m_scanlineCounter(0)
    , m_screenMode(0)
    , m_frameCycleCount(4000000/50)
    , m_cpu(cpu)
    , m_crtc(crtc)
    , m_renderer(0)
{
    connect(m_crtc, SIGNAL(hSync(bool)),
            this, SLOT(hSync(bool)));
    connect(m_crtc, SIGNAL(vSync(bool)),
            this, SLOT(vSync(bool)));
}

void GateArray::run()
{
    int cycleCount = m_cpu->step();

    // wait states
    if (cycleCount)
    {
        int cycles = cycleCount >> 2;
        while (cycles)
        {
            m_crtc->run();
            word_t videoAddress = ((m_crtc->memoryAddress() & 0x3000) << 2)  // MA13 & MA12
                                | ((m_crtc->rowAddress() & 0x07) << 11)      // RA2 - RA0
                                | ((m_crtc->memoryAddress() & 0x03ff) << 1); // MA9 - MA0

            byte_t displayByte1 = Memory::ram[videoAddress++];
            byte_t displayByte2 = Memory::ram[videoAddress];

            if (m_renderer)
            {
                m_renderer->draw(displayByte1, displayByte2);
            }

            cycles--;
        }
        m_frameCycleCount -= cycleCount;
    }

    m_cpu->checkInterrupt();

    if (m_frameCycleCount <= 0)
    {
        m_frameCycleCount += 4000000/50;
    }
}

void GateArray::setRenderer(ScreenRenderer* renderer)
{
    m_renderer = renderer;
}

bool GateArray::in(word_t address, byte_t& value)
{
    Q_UNUSED(address)
    Q_UNUSED(value)

    return false;
}

bool GateArray::out(word_t address, byte_t value)
{
    bool handled = false;

    // 0x7Fxx: gate array port address (bit 15 = 0, bit 14 = 1)
    if ((address & 0xc000) == 0x4000)
    {
        // bit 7 and 6 define the function selected
        switch (value & 0xc0)
        {
            // bit 7 = 0, bit 6 = 0: select pen
            case 0x00:
                selectPen(value);
                break;

            // bit 7 = 0, bit 6 = 1: select color
            case 0x40:
                selectColorForPen(value);
                break;

            // bit 7 = 1, bit 6 = 0: select screen mode, rom configuration and interrupt control
            case 0x80:
                setRomConfiguration(value);
                break;
        }

        handled = true;
    }

    if (handled)
    {
        qDebug() << "[GA  ] OUT request at address" << hex << address << "with value" << hex << value;
    }

    return handled;
}

void GateArray::hSync(bool active)
{
    if (active)
    {
        m_scanlineCounter++;

        if (m_scanlineCounter >= 52)
        {
            m_cpu->setInterruptPending();
            m_scanlineCounter = 0;
        }

        m_renderer->hSync();
    }
}

void GateArray::vSync(bool active)
{
    m_renderer->vSync(active);
}

void GateArray::selectPen(byte_t value)
{
    // bit 4 set --> change border ink
    if( value & 0x10 )
        m_currentPen = 16;
    else
        m_currentPen = value & 0x0f;

    qDebug() << "[GA  ] select pen" << m_currentPen;
}

void GateArray::selectColorForPen(byte_t value)
{
    m_inkValues[m_currentPen] = value & 0x1f;
    m_renderer->setColor(m_currentPen, value & 0x1f);
    qDebug() << "[GA  ] select color" << m_inkValues[m_currentPen] << "for pen" << m_currentPen;
}

void GateArray::setRomConfiguration(byte_t value)
{
    Memory memory;

    // bit 0 & 1: screen mode selection
    m_screenMode = value & 0x03;

    qDebug() << "[GA  ] set screen mode to" << m_screenMode;

    // bit 2: 0=enabled 1=disabled
    bool lowerRomEnabled = !(value & 0x04);
    memory.blocks[0] = lowerRomEnabled ? memory.kernelRom
                                       : memory.ram;

    qDebug() << "[GA  ] set ROM configuration: lower ROM enabled is" << lowerRomEnabled;

    // bit 3: 0=enabled 1=disabled
    bool upperRomEnabled = !(value & 0x08);
    memory.blocks[3] = upperRomEnabled ? memory.basicRom
                                       : memory.ram + 0xc000;

    qDebug() << "[GA  ] set ROM configuration: upper ROM enabled is" << upperRomEnabled;

    // bit 4: 1=clear scan line counter
    if (value & 0x10)
    {
        m_scanlineCounter = 0;
        qDebug() << "[GA  ] clear scan line counter to delay interrupt generation";
    }
}
