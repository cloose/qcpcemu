#include "gatearray.h"

#include <QDebug>

#include "memory.h"


GateArray::GateArray()
    : m_currentPen(0)
    , m_scanlineCounter(0)
    , m_screenMode(0)
{
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
}
