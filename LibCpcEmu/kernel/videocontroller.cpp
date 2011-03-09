#include "videocontroller.h"
#include "videocontroller_p.cpp"

#include <QDebug>


VideoController::VideoController(QObject* parent)
    : QObject(parent)
    , d(new VideoControllerPrivate(this))
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
        run();
        cycles--;
    }
}

void VideoController::run()
{
    d->horizontalCounter++;

    // R0: reached horizontal total?
    d->horizontalTotalCoincidence();

    d->updateHorizontalSyncWidthCounter();

    // R3: horizontal sync width
    d->horizontalSyncWidthCoincidence();

    // R1: reached horizontal displayed?
    d->horizontalDisplayedCoincidence();

    // R2: reached horizontal sync position?
    d->horizontalSyncPositionCoincidence();

    d->linearAddressGenerator();
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
            value   = d->readRegisterValue();
            handled = true;
            qDebug() << "[CRTC] get value" << hex << value << "from register" << d->addressRegister;
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
            d->addressRegister = value;
            handled = true;
            qDebug() << "[CRTC] select register" << value;
            break;

        // 0xbd00: write 6845 register data
        case 0x0100:
            d->writeRegisterValue(value);
            handled = true;
            qDebug() << "[CRTC] set register" << d->addressRegister << "to value" << hex << value;
            break;
    }

    if (handled)
    {
        qDebug() << "[CRTC] OUT request at address" << hex << address << "with value" << hex << value;
    }

    return handled;
}

word_t VideoController::memoryAddress() const
{
    return d->memoryAddress;
}

byte_t VideoController::rowAddress() const
{
    return d->rasterAddress;
}

bool VideoController::displayEnabled() const
{
    return d->horizontalDisplayEnabled && d->verticalDisplayEnabled;
}
