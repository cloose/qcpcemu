#include "videocontroller.h"
#include "videocontroller_p.cpp"

#include <QDebug>


VideoController::VideoController()
    : d(new VideoControllerPrivate)
{
}

VideoController::~VideoController()
{
    delete d;
}

void VideoController::run(int cycles)
{
    while (cycles)
    {
        if (d->horizontalCounter >= d->registers[0])
        {
            d->horizontalCounter = 0;
        }

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

        // 0xbf00: read from current register
        case 0x0300:
            value   = readRegisterValue();
            handled = true;
            qDebug() << "[CRTC] get value" << hex << value << "from register" << d->currentRegister;
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
            d->currentRegister = value;
            handled = true;
            qDebug() << "[CRTC] select register" << value;
            break;

        // 0xbd00: write 6845 register data
        case 0x0100:
            writeRegisterValue(value);
            handled = true;
            qDebug() << "[CRTC] set register" << d->currentRegister << "to value" << hex << value;
            break;
    }

    if (handled)
    {
        qDebug() << "[CRTC] OUT request at address" << hex << address << "with value" << hex << value;
    }

    return handled;
}

byte_t VideoController::readRegisterValue()
{
    if ((d->currentRegister > 11) && (d->currentRegister < 18))
    {
        return d->registers[d->currentRegister];
    }
    else
    {
        // write-only registers return 0
        return 0;
    }
}

void VideoController::writeRegisterValue(byte_t value)
{
    if  (d->currentRegister < 16)
    {
        switch (d->currentRegister)
        {
            case 0: // horizontal total
                d->registers[0] = value;
                qDebug() << "[CRTC] set horizontal total to" << value;
                break;
            case 1: // horizontal displayed
                d->registers[1] = value;
                qDebug() << "[CRTC] set horizontal displayed to" << value;
                break;
            case 2: // horizontal sync position
                d->registers[2] = value;
                qDebug() << "[CRTC] set horizontal sync position to" << value;
                break;
            case 3: // sync width
                d->registers[3] = value;
                break;
            case 4: // vertical total
                d->registers[4] = value & 0x7f;
                qDebug() << "[CRTC] set vertical total to" << value;
                break;
            case 5: // vertical total adjust
                d->registers[5] = value & 0x1f;
                qDebug() << "[CRTC] set vertical total adjust to" << value;
                break;
            case 6: // vertical displayed
                d->registers[6] = value & 0x7f;
                qDebug() << "[CRTC] set vertical displayed to" << value;
                break;
            case 12: // start address high byte
                d->registers[12] = value & 0x3f;
                break;
            case 13: // start address low byte
                d->registers[13] = value;
                break;
            case 14: // cursor address high byte
                d->registers[14] = value & 0x3f;
                break;
            case 15: // cursor address low byte
                d->registers[15] = value;
                break;
            default:
                d->registers[d->currentRegister] = value;
                break;
        }
    }
    else
        qCritical() << "[CRTC] write to crtc register >= 16:" << d->currentRegister;
}
