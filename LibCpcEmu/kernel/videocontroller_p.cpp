#include "videocontroller.h"

#include <QDebug>

class VideoControllerPrivate
{
public:
    VideoControllerPrivate(VideoController* crtc)
        : q(crtc)
        , addressRegister(0)
        , horizontalCounter(0)
        , hSyncCounter(0)
        , hSyncActive(false)
        , scanLineCounter(0)
        , vSyncCounter(0)
        , vSyncActive(false)
        , verticalTotalAdjustIndicator(0x00)
        , displayEnabled(0x00)
        , horizontalDisplayEnabled(false)
        , rowAddress(0)
        , rowAddressMask(0x1f)
        , characterAddress(0)
        , characterAddressLatch(0)
        , oddField(false)
    {
    }

    void horizontalRetrace();
    void checkVerticalRetrace();

    VideoController* q;

    // 5-bit register used to select 18 internal control registers (R0-R17)
    quint8 addressRegister;
    byte_t registers[18];

    quint8 horizontalCounter;

    // HSYNC
    quint16 hSyncCounter;
    bool hSyncActive;

    quint8 scanLineCounter;

    // VSYNC
    quint16 vSyncCounter;
    bool vSyncActive;

    // 0x00 = last character row was not reached yet
    // 0x80 = start vertical total adjust in next line
    // 0x01 = vertical total adjust is active
    byte_t verticalTotalAdjustIndicator;

    // bit 1: 1: display enabled (b6=1 and b7=1)
    // bit 6: 1: display enabled H
    // bit 7: 1: display enabled V
    byte_t displayEnabled;

    bool horizontalDisplayEnabled;

    // RA0-RA4 row address
    // TODO: better name
    byte_t rowAddress;
    // 0x1e if R8 == 3, otherwise 0x1f (interlace mode)
    byte_t rowAddressMask;

    // MA0-MA13
    // TODO: better name
    word_t characterAddress;
    word_t characterAddressLatch;

    // for interlace
    bool oddField;
};

// This 8-bit register contains the total of displayed and non-displayed
// characters, minus one, per horizontal line. The frequency of HSYNC is thus
// determined by this register.
#define HORIZONTAL_TOTAL d->registers[0]

// This 8-bit register contains the number of displayed characters
// per horizontal line
#define HORIZONTAL_DISPLAYED d->registers[1]

// This 8-bit register contains the position of the HSYNC on the horizontal
// line, in terms of the character location number on the line. The position
// of the HSYNC determines the left-to-right location of the displayed text
// on the video screen. In this way, the side margins are adjusted.
#define HORIZONTAL_SYNC_POSITION d->registers[2]

// This 8-bit register contains the widths of both HSYNC and VSYNC, as follows:
//     7 6 5 4  3 2 1 0
//     | | | |  | | | |
//     8 4 2 1  8 4 2 1
//    \-------/\-------/
//        |        |
//      VSYNC    HSYNC
//
// If bits 4-7 are all "0", then VSYNC will be 16 scan lines wide.
#define SYNC_WIDTHS d->registers[3]

// The Vertical Total Register is a 7-bit register containing the total
// number of character rows in a frame, minus one. This register, along with
// R5, determines the overall frame rate, which should be close to the line
// frequency to ensure flicker-free appearance.
#define VERTICAL_TOTAL d->registers[4]

// This 5-bit register contains the number of scan lines per character row,
// including spacing, minus one.
#define MAX_SCAN_LINE_ADDRESS d->registers[9]

void VideoControllerPrivate::horizontalRetrace()
{
    horizontalCounter = 0;

    displayEnabled = (displayEnabled | 0x40)
                   | ((displayEnabled & 0x80) >> 6);
    horizontalDisplayEnabled = true;

    characterAddress = characterAddressLatch;

    if (!((rowAddress  ^ registers[11]) & rowAddressMask))
    {
        // TODO
    }

    if (verticalTotalAdjustIndicator)
    {
        // TODO
        checkVerticalRetrace();
    }
    else if (!((rowAddress ^ registers[9]) & rowAddressMask))
    {
        // ??? Auf Null setzen ???
        rowAddress = (~rowAddress) & oddField;
        scanLineCounter++;
        if (!scanLineCounter)
        {
            characterAddress = (registers[13]) | ((registers[12] & 0x3f) << 8);
            characterAddressLatch = characterAddress;
        }
    }
    else
    {
        // increment row address ???
        rowAddress = (rowAddress - rowAddressMask) & 0x1f;
        if (scanLineCounter == registers[4])
        {
            if (!((rowAddress ^ registers[9]) & rowAddressMask))
            {
                verticalTotalAdjustIndicator = 0x80;
                // TODO
            }
        }
    }

    if (!((rowAddress ^ registers[10]) & rowAddressMask))
    {
        // TODO
    }

    if (vSyncActive)
    {
        vSyncCounter++;
        if (!((vSyncCounter ^ (registers[3] >> 4)) & 0x0f))
        {
            q->deactivateVSyncSignal();
        }
    }

    if (scanLineCounter == registers[6])
    {
        displayEnabled = 0x40;
    }

    if (scanLineCounter == registers[7])
    {
        if ((rowAddress & rowAddressMask) == 0)
        {
            // TODO
            q->activateVSyncSignal();
            vSyncCounter = 0;
        }
    }
    qDebug() << "[CRTC] horizontal retrace MISSING IMPLEMENTATION";
}

void VideoControllerPrivate::checkVerticalRetrace()
{
    if (verticalTotalAdjustIndicator & 0x80)
    {
        verticalTotalAdjustIndicator = 0x01;
    }
    else
    {

    }

    verticalTotalAdjustIndicator = 0x00;
    scanLineCounter = 0;
    characterAddress = (registers[13]) | ((registers[12] & 0x3f) << 8);
    characterAddressLatch = characterAddress;

    qDebug() << "[CRTC] vertical retrace MISSING IMPLEMENTATION";
}
