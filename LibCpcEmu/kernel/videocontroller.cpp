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
    // passed horizontal total?
    if (d->horizontalCounter >= HORIZONTAL_TOTAL)
    {
        d->horizontalRetrace();
    }
    else
    {
        d->horizontalCounter++;
        d->characterAddress++;
    }

    // is HSYNC signal active?
    if (d->hSyncActive)
    {
        d->hSyncCounter++;
        if (!((d->hSyncCounter ^ SYNC_WIDTHS) & 0x0f))
        {
            deactivateHSyncSignal();
        }
    }

    if (d->horizontalCounter == HORIZONTAL_DISPLAYED)
    {
        // outside of horizontal display area
        d->displayEnabled = d->displayEnabled & 0x80;
        d->horizontalDisplayEnabled = false;

        // max scan line reached?
        if (!((d->rowAddress ^ MAX_SCAN_LINE_ADDRESS) & d->rowAddressMask))
        {
            d->characterAddressLatch = d->characterAddress;
        }
    }

    // reached Horizontal Sync position?
    if (d->horizontalCounter == HORIZONTAL_SYNC_POSITION)
    {
        activateHSyncSignal();
        d->hSyncCounter = 0;
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
            writeRegisterValue(value);
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
    return d->characterAddress;
}

byte_t VideoController::rowAddress() const
{
    return d->rowAddress;
}

byte_t VideoController::readRegisterValue()
{
    if (d->addressRegister > 11 && d->addressRegister < 18)
    {
        return d->registers[d->addressRegister];
    }
    else
    {
        // write-only registers return 0
        return 0;
    }
}

void VideoController::writeRegisterValue(byte_t value)
{
    if  (d->addressRegister < 16)
    {
        switch (d->addressRegister)
        {
            case 0: // horizontal total
                HORIZONTAL_TOTAL = value;
                qDebug() << "[CRTC] set horizontal total to" << value;
                break;
            case 1: // horizontal displayed
                HORIZONTAL_DISPLAYED = value;
                qDebug() << "[CRTC] set horizontal displayed to" << value;
                break;
            case 2: // horizontal sync position
                HORIZONTAL_SYNC_POSITION = value;
                qDebug() << "[CRTC] set horizontal sync position to" << value;
                break;
            case 3: // horizontal and vertical sync widths
                d->registers[3] = value;
                qDebug() << "[CRTC] set horizontal sync width to" << (value & 0x0f);
                qDebug() << "[CRTC] set vertical sync width to" << ((value >> 4) ? (value >> 4) : 16);
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
                // TODO: check scan line counter
                qDebug() << "[CRTC] set vertical displayed to" << value;
                break;
            case 7: // vertical sync position
                d->registers[7] = value & 0x7f;
                // TODO: check scan line counter
                qDebug() << "[CRTC] set vertical sync position to" << value;
                break;
            case 8: // interlace mode and skew
                d->registers[8] = value & 0xf3;
                d->rowAddressMask = (value & 0x03 ? 0x1e : 0x1f);
                // TODO
                break;
            case 9: // max scan line address
                d->registers[9] = value & 0x1f;
                break;
            case 10: // cursor start
                d->registers[10] = value & 0x7f;
                break;
            case 11: // cursor end
                d->registers[11] = value & 0x1f;
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
                // TODO
                break;
            default:
                d->registers[d->addressRegister] = value;
                break;
        }
    }
    else
        qCritical() << "[CRTC] write to crtc register >= 16:" << d->addressRegister;
}

void VideoController::activateHSyncSignal()
{
    if (!d->hSyncActive)
    {
        d->hSyncActive = true;
        emit hSync(d->hSyncActive);
        qDebug() << "[CRTC] ***HSYNC ACTIVE***";
    }
}

void VideoController::deactivateHSyncSignal()
{
    if (d->hSyncActive)
    {
        d->hSyncActive = false;
        emit hSync(d->hSyncActive);
        qDebug() << "[CRTC] ***HSYNC DEACTIVE***";
    }
}

void VideoController::activateVSyncSignal()
{
    if (!d->vSyncActive)
    {
        d->vSyncActive = true;
        emit vSync(d->vSyncActive);
        qDebug() << "[CRTC] ***VSYNC ACTIVE***";
    }
}

void VideoController::deactivateVSyncSignal()
{
    if (d->vSyncActive)
    {
        d->vSyncActive = false;
        emit vSync(d->vSyncActive);
        qDebug() << "[CRTC] ***VSYNC DEACTIVE***";
    }
}
