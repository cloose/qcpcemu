#include "videocontroller.h"

#include <QDebug>


// make a word out of two bytes
#define WORD(low, high)     (low | (high << 8))

// This 8-bit register contains the total of displayed and non-displayed
// characters, minus one, per horizontal line. The frequency of HSYNC is thus
// determined by this register.
#define HORIZONTAL_TOTAL registers[0]

// This 8-bit register contains the number of displayed characters
// per horizontal line
#define HORIZONTAL_DISPLAYED registers[1]

// This 8-bit register contains the position of the HSYNC on the horizontal
// line, in terms of the character location number on the line. The position
// of the HSYNC determines the left-to-right location of the displayed text
// on the video screen. In this way, the side margins are adjusted.
#define HORIZONTAL_SYNC_POSITION registers[2]

// This 8-bit register contains the widths of both HSYNC and VSYNC, as follows:
//     7 6 5 4  3 2 1 0
//     | | | |  | | | |
//     8 4 2 1  8 4 2 1
//    \-------/\-------/
//        |        |
//      VSYNC    HSYNC
//
// If bits 4-7 are all "0", then VSYNC will be 16 scan lines wide.
#define SYNC_WIDTHS registers[3]
#define HORIZONTAL_SYNC_WIDTH (registers[3] & 0x0f)
#define VERTICAL_SYNC_WIDTH ((registers[3] >> 4) & 0x0f)

// TODO: missing description
#define VERTICAL_TOTAL registers[4]

// TODO: missing description
#define VERTICAL_DISPLAYED registers[6]

// TODO: missing description
#define VERTICAL_SYNC_POSITION registers[7]

// This 5-bit register contains the number of scan lines per character row,
// including spacing, minus one.
#define MAX_SCAN_LINE_ADDRESS registers[9]


class VideoControllerPrivate
{
public:
    VideoControllerPrivate(VideoController* crtc)
        : horizontalCounter(0)
        , horizontalSyncWidthCounter(0)
        , scanLineCounter(0)
        , characterRowCounter(0)
        , verticalSyncWidthCounter(0)
        , hSyncActive(false)
        , vSyncActive(false)
        , horizontalDisplayEnabled(false)
        , verticalDisplayEnabled(false)
        , verticalTotalReached(false)
        , verticalDisplayedReached(false)
        , verticalSyncPositionReached(false)
        , rasterAddress(0)
        , memoryAddress(0)
        , addressRegister(0)
        , q(crtc)
    {
    }

    word_t startAddress() const { return WORD(registers[13], (registers[12] & 0x3f)); }
    word_t cursorLocation() const { return WORD(registers[15], (registers[14] & 0x3f)); }

    byte_t readRegisterValue();
    void writeRegisterValue(byte_t value);

    void horizontalTotalCoincidence();
    void horizontalDisplayedCoincidence();
    void horizontalSyncPositionCoincidence();
    void horizontalSyncWidthCoincidence();
    void maxScanLineCoincidence();
    void verticalTotalCoincidence();
    void verticalDisplayedCoincidence();
    void verticalSyncPositionCoincidence();

    void updateHorizontalSyncWidthCounter();
    void horizontalClock();

    void verticalControl();
    void linearAddressGenerator();

    quint8 horizontalCounter;
    quint8 horizontalSyncWidthCounter;
    quint8 scanLineCounter;
    quint8 characterRowCounter;
    quint8 verticalSyncWidthCounter;

    bool hSyncActive;
    bool vSyncActive;

    bool horizontalDisplayEnabled;
    bool verticalDisplayEnabled;

    bool verticalTotalReached;
    bool verticalDisplayedReached;
    bool verticalSyncPositionReached;

    // RA0-RA4: raster address
    byte_t rasterAddress;

    // MA0-MA13: 14-bit memory address
    word_t memoryAddress;

    // 5-bit register used to select 18 internal control registers (R0-R17)
    quint8 addressRegister;
    byte_t registers[18];

private:
    VideoController* q;
};


byte_t VideoControllerPrivate::readRegisterValue()
{
    if (addressRegister > 11 && addressRegister < 18)
    {
        return registers[addressRegister];
    }
    else
    {
        // write-only registers return 0
        return 0;
    }
}

void VideoControllerPrivate::writeRegisterValue(byte_t value)
{
    if  (addressRegister < 16)
    {
        switch (addressRegister)
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
                SYNC_WIDTHS = value;
                qDebug() << "[CRTC] set horizontal sync width to" << (value & 0x0f);
                qDebug() << "[CRTC] set vertical sync width to" << ((value >> 4) ? (value >> 4) : 16);
                break;
            case 4: // vertical total
                VERTICAL_TOTAL = value & 0x7f;
                qDebug() << "[CRTC] set vertical total to" << value;
                break;
            case 5: // vertical total adjust
                registers[5] = value & 0x1f;
                qDebug() << "[CRTC] set vertical total adjust to" << value;
                break;
            case 6: // vertical displayed
                VERTICAL_DISPLAYED = value & 0x7f;
                // TODO: check scan line counter
                qDebug() << "[CRTC] set vertical displayed to" << value;
                break;
            case 7: // vertical sync position
                VERTICAL_SYNC_POSITION = value & 0x7f;
                // TODO: check scan line counter
                qDebug() << "[CRTC] set vertical sync position to" << value;
                break;
            case 8: // interlace mode and skew
                registers[8] = value & 0xf3;
//                d->rowAddressMask = (value & 0x03 ? 0x1e : 0x1f);
                // TODO
                break;
            case 9: // max scan line address
                MAX_SCAN_LINE_ADDRESS = value & 0x1f;
                break;
            case 10: // cursor start
                registers[10] = value & 0x7f;
                break;
            case 11: // cursor end
                registers[11] = value & 0x1f;
                break;
            case 12: // start address high byte
                registers[12] = value & 0x3f;
                break;
            case 13: // start address low byte
                registers[13] = value;
                break;
            case 14: // cursor address high byte
                registers[14] = value & 0x3f;
                break;
            case 15: // cursor address low byte
                registers[15] = value;
                // TODO
                break;
            default:
                registers[addressRegister] = value;
                break;
        }
    }
    else
        qCritical() << "[CRTC] write to crtc register >= 16:" << addressRegister;
}

void VideoControllerPrivate::horizontalTotalCoincidence()
{
    // R0: reached horizontal total?
    if (horizontalCounter >= (HORIZONTAL_TOTAL + 1))
    {
        horizontalCounter = 0;
        horizontalDisplayEnabled = true;

        horizontalClock();
    }
}

void VideoControllerPrivate::horizontalDisplayedCoincidence()
{
    // R1: reached horizontal displayed?
    if (horizontalCounter == HORIZONTAL_DISPLAYED)
    {
        horizontalDisplayEnabled = false;
    }
}

void VideoControllerPrivate::horizontalSyncPositionCoincidence()
{
    // R2: reached horizontal sync position?
    if (horizontalCounter == HORIZONTAL_SYNC_POSITION)
    {
        if (!hSyncActive)
        {
            hSyncActive = true;
            emit q->hSync(hSyncActive);
        }
    }
}

void VideoControllerPrivate::horizontalSyncWidthCoincidence()
{
    // R3: horizontal sync width
    if (horizontalSyncWidthCounter == HORIZONTAL_SYNC_WIDTH)
    {
        horizontalSyncWidthCounter = 0;

        if (hSyncActive)
        {
            hSyncActive = false;
            emit q->hSync(hSyncActive);
        }
    }
}

void VideoControllerPrivate::maxScanLineCoincidence()
{
    // R9: reached max scan line?
    if (scanLineCounter == (MAX_SCAN_LINE_ADDRESS+1))
    {
        scanLineCounter = 0;
        characterRowCounter++;
    }
}

void VideoControllerPrivate::verticalTotalCoincidence()
{
    // R4: reached vertical total?
    verticalTotalReached = (characterRowCounter == (VERTICAL_TOTAL + 1));
}

void VideoControllerPrivate::verticalDisplayedCoincidence()
{
    // R6: reached vertical displayed?
    verticalDisplayedReached = (characterRowCounter == VERTICAL_DISPLAYED);
}

void VideoControllerPrivate::verticalSyncPositionCoincidence()
{
    // R7: reached vertical sync position?
    verticalSyncPositionReached = (characterRowCounter == VERTICAL_SYNC_POSITION);
}

void VideoControllerPrivate::updateHorizontalSyncWidthCounter()
{
    if (hSyncActive)
    {
        horizontalSyncWidthCounter++;
    }
}

void VideoControllerPrivate::horizontalClock()
{
    scanLineCounter++;

    // R9: reached max scan line?
    maxScanLineCoincidence();

    // R4: reached vertical total?
    verticalTotalCoincidence();

    // R6: reached vertical displayed?
    verticalDisplayedCoincidence();

    // R7: reached vertical sync position?
    verticalSyncPositionCoincidence();

    verticalControl();
}

void VideoControllerPrivate::verticalControl()
{
    if (verticalDisplayedReached && scanLineCounter >= MAX_SCAN_LINE_ADDRESS)
    {
        verticalDisplayEnabled = false;
    }

    if (verticalTotalReached && scanLineCounter >= MAX_SCAN_LINE_ADDRESS)
    {
        verticalDisplayEnabled = true;
        characterRowCounter = 0;
        scanLineCounter = 0;
        emit q->endOfFrame();
    }

    if (vSyncActive)
    {
        verticalSyncWidthCounter++;

        // R4: vertical sync width
        if (verticalSyncWidthCounter == VERTICAL_SYNC_WIDTH)
        {
            verticalSyncWidthCounter = 0;

            if (vSyncActive)
            {
                vSyncActive = false;
                emit q->vSync(vSyncActive);
            }
        }
    }

    if (verticalSyncPositionReached && !vSyncActive)
    {
        vSyncActive = true;
        emit q->vSync(vSyncActive);
    }

    rasterAddress = scanLineCounter;
}

void VideoControllerPrivate::linearAddressGenerator()
{
    memoryAddress = startAddress()
                  + (characterRowCounter * HORIZONTAL_DISPLAYED)
                  + horizontalCounter;
}
