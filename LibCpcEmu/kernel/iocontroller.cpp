#include "iocontroller.h"

#include <QDebug>


IoController::IoController()
    : m_portA(0x00)
    , m_portB(0x00)
    , m_portC(0x00)
    , m_control(0x00)
{
}

bool IoController::in(word_t address, byte_t& value)
{
    bool handled = false;

    // bit 8, 9 and 11
    switch (address & 0x0b00)
    {
        // 0xf400: 8255 port A (AY-3-8912 PSG data bus)
        case 0x0000:
            handled = true;
            // TODO: missing implementation
            break;

        // 0xf500: 8255 port B
        case 0x0100:
            // bit 0  : VSYNC of CRTC
            // bit 1-3: manufacturer id
            // bit 4  : 50 / 60 Hz selection
            // bit 5  :
            // bit 6  : printer status
            // bit 7  : recorder data
            if (m_control & 0x02)   // port B set to input?
            {
                // TODO: missing implementation
                value = m_portB;
            }
            else
            {
                // return last programmed value
                value = m_portB;
            }

            handled = true;
            break;

        // 0xf600: 8255 port C (AY-3-8912 PSG control)
        case 0x0200:
            handled = true;
            // TODO: missing implementation
            break;

        // 0xf700: 8255 control register
        case 0x0300:
            handled = true;
            // TODO: missing implementation
            break;
    }

    if (handled)
    {
        qDebug() << "[PPI ] IN request at address" << hex << address << "returned value" << hex << value;
    }

    return handled;
}

bool IoController::out(word_t address, byte_t value)
{
    bool handled = false;

    // bit 8, 9 and 11
    switch (address & 0x0b00)
    {
        // 0xf400: 8255 port A (AY-3-8912 PSG data bus)
        case 0x0000:
            m_portA = value;
            handled = true;

            // TODO: sound card
            break;

        // 0xf500: 8255 port B
        case 0x0100:
            Q_ASSERT(false);
            break;

        // 0xf600: 8255 port C (AY-3-8912 PSG control)
        case 0x0200:
            m_portC = value;
            handled = true;

            // TODO: keyboard
            // TODO: sound card
            break;

        // 0xf700: 8255 control register
        case 0x0300:
            if (value & 0x80)
            {
                m_control = value;
                handled = true;

                // TODO: is this correct?
                m_portA = 0x00;
                m_portB = 0x00;
                m_portC = 0x00;
            }
            else
            {
                // TODO
            }
            break;
    }

    if (handled)
    {
        qDebug() << "[PPI ] OUT request at address" << hex << address << "with value" << hex << value;
    }

    return handled;
}
