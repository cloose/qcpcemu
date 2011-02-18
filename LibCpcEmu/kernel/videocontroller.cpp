#include "videocontroller.h"

#include <QDebug>


VideoController::VideoController()
    : m_currentRegister(0)
{
}

void VideoController::run(int cycles)
{
    while (cycles)
    {

        cycles--;
    }
}

bool VideoController::in(word_t address, byte_t& value)
{
    bool handled = false;

    // bit 8, 9 and 14
    switch (address & 0x4300)
    {
        // 0xbe00: ???
        case 0x0200:
            handled = true;
            // TODO: missing implementation
            break;

        // 0xbf00: ???
        case 0x0300:
            handled = true;
            // TODO: missing implementation
            break;
    }

    if (handled)
    {
        qDebug() << "[CRTC] IN request at address" << hex << address << "returned value" << hex << value;
    }

    return handled;
}

bool VideoController::out(word_t address, byte_t value)
{
    bool handled = false;

    // bit 8, 9 and 14
    switch (address & 0x4300)
    {
        // 0xbc00: select 6845 register
        case 0x0000:
            m_currentRegister = value;
            handled = true;
            qDebug() << "[CRTC] select register" << value;
            break;

        // 0xbd00: write 6845 register data
        case 0x0100:
            m_registers[m_currentRegister] = value;
            handled = true;
            qDebug() << "[CRTC] set register" << m_currentRegister << "to value" << hex << value;
            // TODO: missing implementation
            break;
    }

    if (handled)
    {
        qDebug() << "[CRTC] OUT request at address" << hex << address << "with value" << hex << value;
    }

    return handled;
}
