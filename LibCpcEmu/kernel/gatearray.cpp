#include "gatearray.h"

#include <QDebug>

#include "memory.h"


GateArray::GateArray(Memory* memory)
    : m_memory(memory)
{
}

bool GateArray::in(word_t address, byte_t& value)
{
    qDebug() << "[GA ] IN request at address" << hex << address;

    return false;
}

bool GateArray::out(word_t address, byte_t value)
{
    bool handled = false;

    // 0x7Fxx: gate array port address (bit 15 = 0, bit 14 = 1)
    if ((address & 0xc000) == 0x4000)
    {
        qDebug() << "[GA ] OUT request at address" << hex << address << "with value" << hex << value;

        // bit 7 and 6 define the function selected
        switch (value & 0xc0)
        {
            // bit 7 = 1, bit 6 = 0: select screen mode, rom configuration and interrupt control
            case 0x80:
                setRomConfiguration(value);
                break;
        }
    }

    return handled;
}

void GateArray::setRomConfiguration(byte_t value)
{
    // bit 2: 0=enabled 1=disabled
    bool lowerRomEnabled = !(value & 0x04);
    m_memory->blocks[0] = lowerRomEnabled ? m_memory->kernelRom
                                          : m_memory->ram;

    qDebug() << "[GA ] set ROM configuration: lower ROM enabled is" << lowerRomEnabled;

    // bit 3: 0=enabled 1=disabled
    bool upperRomEnabled = !(value & 0x08);
    m_memory->blocks[3] = upperRomEnabled ? m_memory->basicRom
                                          : m_memory->ram + 0xc000;

    qDebug() << "[GA ] set ROM configuration: upper ROM enabled is" << upperRomEnabled;
}
