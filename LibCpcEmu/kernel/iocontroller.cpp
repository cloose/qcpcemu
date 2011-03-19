#include "iocontroller.h"

#include <QDebug>

#include "exceptions.h"
#include "keyboard.h"


IoController::IoController(Keyboard* keyboard, QObject* parent)
    : QObject(parent)
    , m_portA(0x00)
    , m_portB(0x00)
    , m_portC(0x00)
    , m_control(0x00)
    , m_vsyncActive(false)
    , m_keyboard(keyboard)
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

            // bit 4: port A set to input?
            if (m_control & 0x10)
            {
                value = m_keyboard->pressedKey();
                // TODO: missing implementation
            }
            else
            {
                // return last programmed value
                value = m_portA;
            }
            break;

        // 0xf500: 8255 port B
        case 0x0100:
            // bit 0  : VSYNC of CRTC
            // bit 1-3: manufacturer id
            // bit 4  : 50 / 60 Hz selection
            // bit 5  :
            // bit 6  : printer status
            // bit 7  : recorder data
            if (m_control & 0x02)   // bit 1: port B set to input?
            {
                // TODO: missing implementation
                // TODO: make manufacturer and screen refresh rate variable
                m_portB = m_vsyncActive
                        | SCHNEIDER
                        | (Refresh50Hz << 4);
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

            // bit 0 & 3: port C set to input?
            if (m_control & 0x09)
            {
                // TODO: missing implementation
                qDebug() << "[PPI ] MISSING IMPLEMENTATION";
            }
            else
            {
                // return last programmed value
                value = m_portC;
            }
            break;

        // 0xf700: 8255 control register
        case 0x0300:
            Q_ASSERT(false);
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
            qDebug() << "[PPI ] MISSING IMPLEMENTATION";
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
            if (!(m_control & 0x01))    // output low order 4 bits
            {
                m_keyboard->setRow(value & 0x0f);
            }

            // TODO: sound card
            if (!(m_control & 0x08))    // output high order 4 bits
            {
                // TODO: missing implementation
//                qDebug() << "[PPI ] MISSING IMPLEMENTATION";
            }
            break;

        // 0xf700: 8255 control register
        case 0x0300:
            // bit 7  : control word identification flag
            //                0 = bit set/reset for port C
            //                1 = define mode and input/output
            if (value & 0x80)
            {
                // if bit 7 set to 1...
                // bit 6-3: group A control bits
                //     bit 6-5: mode definition of group A
                //                00 = mode 0
                //                01 = mode 1
                //                1x = mode 2
                //     bit 4:   definition of input/output of port A (8 bits)
                //                0 = output, 1=input
                //     bit 3:   definition of input/output of port C (high order 4 bits)
                //                0 = output, 1=input
                // bit 2-0: group B control bits
                //     bit 2:   mode definition of group B
                //                0 = mode 0
                //                1 = mode 1
                //     bit 1:   definition of input/output of port B (8 bits)
                //                0 = output, 1=input
                //     bit 0:   definition of input/output of port C (low order 4 bits)
                //                0 = output, 1=input
                m_control = value;

                // TODO: is this correct?
                m_portA = 0x00;
                m_portB = 0x00;
                m_portC = 0x00;
            }
            else
            {
                // if bit 7 set to 0...
                // bit 6-4: dont's care
                // bit 3-1: definition of bit wanted to be set or reset
                //            000 = PC0
                //            001 = PC1
                //            010 = PC2
                //            011 = PC3
                //            100 = PC4
                //            101 = PC5
                //            110 = PC6
                //            111 = PC7
                // bit 0:   definition of set or reset
                //            0 = reset, 1 = set

                if (value & 0x01)
                {
                    // set bit
                    byte_t bit = (value >> 1) & 0x07;
                    qDebug() << "[PPI] set bit" << bit;
                    m_portC |= (1 << bit);

                    // TODO: output port C?
                    qDebug() << "[PPI ] MISSING IMPLEMENTATION";
                }
                else
                {
                    // reset bit
                    byte_t bit = (value >> 1) & 0x07;
                    qDebug() << "[PPI] reset bit" << bit;
                    m_portC &= ~(1 << bit);

                    // TODO: output port C?
                    qDebug() << "[PPI ] MISSING IMPLEMENTATION";
                }
            }
            handled = true;
            break;
    }

    if (handled)
    {
        qDebug() << "[PPI ] OUT request at address" << hex << address << "with value" << hex << value;
    }

    return handled;
}

void IoController::vSync(bool active)
{
    m_vsyncActive = active;
}
