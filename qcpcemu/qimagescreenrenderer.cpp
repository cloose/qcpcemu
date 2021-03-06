#include "qimagescreenrenderer.h"

#include <QDebug>
#include <QImage>
#include <QWidget>

#include "exceptions.h"

static uchar cpc_palette[32][3] =
    {
        { 0x7f, 0x7f, 0x7f }, // White
        { 0x7f, 0x7f, 0x7f }, // White (not official)
        { 0x00, 0xff, 0x7f }, // Sea Green
        { 0xff, 0xff, 0x7f }, // Pastel Yellow
        { 0x00, 0x00, 0x7f }, // Blue
        { 0xff, 0x00, 0x7f }, // Purple
        { 0x00, 0x7f, 0x7f }, // Cyan
        { 0xff, 0x7f, 0x7f }, // Pink
        { 0xff, 0x00, 0x7f }, // Purple (not official)
        { 0xff, 0xff, 0x7f }, // Pastel Yellow (not official)
        { 0xff, 0xff, 0x00 }, // Bright Yellow
        { 0xff, 0xff, 0xff }, // Bright White
        { 0xff, 0x00, 0x00 }, // Bright Red
        { 0xff, 0x00, 0xff }, // Bright Magenta
        { 0xff, 0x7f, 0x00 }, // Orange
        { 0xff, 0x7f, 0xff }, // Pastel Magenta
        { 0x00, 0x00, 0x7f }, // Blue (not official)
        { 0x00, 0xff, 0x7f }, // Sea Green (not official)
        { 0x00, 0xff, 0x00 }, // Bright Green
        { 0x00, 0xff, 0xff }, // Bright Cyan
        { 0x00, 0x00, 0x00 }, // Black
        { 0x00, 0x00, 0xff }, // Bright Blue
        { 0x00, 0x7f, 0x00 }, // Green
        { 0x00, 0x7f, 0xff }, // Sky Blue
        { 0x7f, 0x00, 0x7f }, // Magenta
        { 0x7f, 0xff, 0x7f }, // Pastel Green
        { 0x7f, 0xff, 0x00 }, // Lime
        { 0x7f, 0xff, 0xff }, // Pastel Cyan
        { 0x7f, 0x00, 0x00 }, // Red
        { 0x7f, 0x00, 0xff }, // Mauve
        { 0x7f, 0x7f, 0x00 }, // Yellow
        { 0x7f, 0x7f, 0xff }  // Pastel Blue
    };

static uchar mode0[256] =
    {
        0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08,
        0x02, 0x02, 0x0a, 0x0a, 0x02, 0x02, 0x0a, 0x0a,
        0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08,
        0x02, 0x02, 0x0a, 0x0a, 0x02, 0x02, 0x0a, 0x0a,
        0x04, 0x04, 0x0c, 0x0c, 0x04, 0x04, 0x0c, 0x0c,
        0x06, 0x06, 0x0e, 0x0e, 0x06, 0x06, 0x0e, 0x0e,
        0x04, 0x04, 0x0c, 0x0c, 0x04, 0x04, 0x0c, 0x0c,
        0x06, 0x06, 0x0e, 0x0e, 0x06, 0x06, 0x0e, 0x0e,
        0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08,
        0x02, 0x02, 0x0a, 0x0a, 0x02, 0x02, 0x0a, 0x0a,
        0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x08, 0x08,
        0x02, 0x02, 0x0a, 0x0a, 0x02, 0x02, 0x0a, 0x0a,
        0x04, 0x04, 0x0c, 0x0c, 0x04, 0x04, 0x0c, 0x0c,
        0x06, 0x06, 0x0e, 0x0e, 0x06, 0x06, 0x0e, 0x0e,
        0x04, 0x04, 0x0c, 0x0c, 0x04, 0x04, 0x0c, 0x0c,
        0x06, 0x06, 0x0e, 0x0e, 0x06, 0x06, 0x0e, 0x0e,
        0x01, 0x01, 0x09, 0x09, 0x01, 0x01, 0x09, 0x09,
        0x03, 0x03, 0x0b, 0x0b, 0x03, 0x03, 0x0b, 0x0b,
        0x01, 0x01, 0x09, 0x09, 0x01, 0x01, 0x09, 0x09,
        0x03, 0x03, 0x0b, 0x0b, 0x03, 0x03, 0x0b, 0x0b,
        0x05, 0x05, 0x0d, 0x0d, 0x05, 0x05, 0x0d, 0x0d,
        0x07, 0x07, 0x0f, 0x0f, 0x07, 0x07, 0x0f, 0x0f,
        0x05, 0x05, 0x0d, 0x0d, 0x05, 0x05, 0x0d, 0x0d,
        0x07, 0x07, 0x0f, 0x0f, 0x07, 0x07, 0x0f, 0x0f,
        0x01, 0x01, 0x09, 0x09, 0x01, 0x01, 0x09, 0x09,
        0x03, 0x03, 0x0b, 0x0b, 0x03, 0x03, 0x0b, 0x0b,
        0x01, 0x01, 0x09, 0x09, 0x01, 0x01, 0x09, 0x09,
        0x03, 0x03, 0x0b, 0x0b, 0x03, 0x03, 0x0b, 0x0b,
        0x05, 0x05, 0x0d, 0x0d, 0x05, 0x05, 0x0d, 0x0d,
        0x07, 0x07, 0x0f, 0x0f, 0x07, 0x07, 0x0f, 0x0f,
        0x05, 0x05, 0x0d, 0x0d, 0x05, 0x05, 0x0d, 0x0d,
        0x07, 0x07, 0x0f, 0x0f, 0x07, 0x07, 0x0f, 0x0f
    };

static uchar mode1[256] =
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
        0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
    };


QImageScreenRenderer::QImageScreenRenderer(QWidget* widget)
    : m_widget(widget)
    , m_xpos(0)
    , m_ypos(0)
    , m_screenBuffer(new QImage(640, 200, QImage::Format_Indexed8))
    , m_scanLine(0)
    , m_drawFuncPtr(&QImageScreenRenderer::drawMode1)
{
    // initialize inks
    for (int i = 0; i < 17; ++i)
        m_inks[i] = 0x00;

    m_screenBuffer->setColorCount(32);
    for( int i = 0; i < 32; i++ )
    {
        int r = (cpc_palette[i][0]);
        int g = (cpc_palette[i][1]);
        int b = (cpc_palette[i][2]);
        m_screenBuffer->setColor(i, qRgb(r, g, b));
    }

    m_screenBuffer->fill(0x00);
    m_scanLine = m_screenBuffer->scanLine(m_ypos);
}

QImageScreenRenderer::~QImageScreenRenderer()
{
    delete m_screenBuffer; m_screenBuffer = 0;
}

const QImage QImageScreenRenderer::screenBuffer() const
{
    return m_screenBuffer->convertToFormat(QImage::Format_ARGB32_Premultiplied);
}

void QImageScreenRenderer::draw(byte_t displayByte1, byte_t displayByte2)
{
//    m_xpos = charPos << 4;
//    if (m_xpos >= WIDTH) return;
//    if (m_ypos >= HEIGHT) return;
    (this->*m_drawFuncPtr)(displayByte1, displayByte2);
}

void QImageScreenRenderer::hSync()
{
    m_ypos++;
    m_xpos = 0;
    m_scanLine = m_screenBuffer->scanLine(m_ypos);
}

void QImageScreenRenderer::vSync(bool active)
{
    if (!active)
    {
        m_ypos = 0;
        m_scanLine = m_screenBuffer->scanLine(m_ypos);

        m_widget->update();
    }
}

void QImageScreenRenderer::setColor(uchar penNum, uchar colorNum)
{
    m_inks[penNum] = colorNum;
//    qDebug() << Q_FUNC_INFO << "pen" << penNum << "has color" << colorNum;
}

QColor QImageScreenRenderer::borderColor() const
{
    return QColor(m_screenBuffer->color(m_inks[16]));
}

void QImageScreenRenderer::setMode(byte_t mode)
{
    qDebug() << "[SR  ] set screen mode to" << mode;

    switch (mode) {
    case 0:
        m_drawFuncPtr = &QImageScreenRenderer::drawMode0;
        break;

    case 1:
        m_drawFuncPtr = &QImageScreenRenderer::drawMode1;
        break;

    case 2:
        m_drawFuncPtr = &QImageScreenRenderer::drawMode2;
        break;

    default:
        m_drawFuncPtr = &QImageScreenRenderer::drawMode1;
        break;
    }
}

// Mode 0 = 2 px per display byte, 16 colors, 160x200 px, 20x25 characters
void QImageScreenRenderer::drawMode0(byte_t displayByte1, byte_t displayByte2)
{
    Q_CHECK_PTR(m_scanLine);

    if (m_xpos >= m_screenBuffer->width()) return;
    if (m_ypos >= m_screenBuffer->height()) return;

    for (int pixel = 0; pixel < 2; ++pixel)
    {
        m_scanLine[m_xpos++]   = m_inks[mode0[displayByte1]];
        m_scanLine[(m_xpos+7)] = m_inks[mode0[displayByte2]];

        m_scanLine[m_xpos++]   = m_inks[mode0[displayByte1]];
        m_scanLine[(m_xpos+7)] = m_inks[mode0[displayByte2]];

        m_scanLine[m_xpos++]   = m_inks[mode0[displayByte1]];
        m_scanLine[(m_xpos+7)] = m_inks[mode0[displayByte2]];

        m_scanLine[m_xpos++]   = m_inks[mode0[displayByte1]];
        m_scanLine[(m_xpos+7)] = m_inks[mode0[displayByte2]];

        displayByte1 <<= 1;
        displayByte2 <<= 1;
    }

    m_xpos += 8;
}

// Mode 1 = 4 px per display byte, 4 colors, 320x200 px, 40x25 characters
void QImageScreenRenderer::drawMode1(byte_t displayByte1, byte_t displayByte2)
{
    Q_CHECK_PTR(m_scanLine);

    if (m_xpos >= m_screenBuffer->width()) return;
    if (m_ypos >= m_screenBuffer->height()) return;

    for (int pixel = 0; pixel < 4; ++pixel)
    {
        m_scanLine[m_xpos++]   = m_inks[mode1[displayByte1]];
        m_scanLine[(m_xpos+7)] = m_inks[mode1[displayByte2]];

        m_scanLine[m_xpos++]   = m_inks[mode1[displayByte1]];
        m_scanLine[(m_xpos+7)] = m_inks[mode1[displayByte2]];

        displayByte1 <<= 1;
        displayByte2 <<= 1;
    }

    m_xpos += 8;
}

// Mode 2 = 8 px per display byte, 2 colors, 640x200 px, 80x25 characters
void QImageScreenRenderer::drawMode2(byte_t displayByte1, byte_t displayByte2)
{
    Q_CHECK_PTR(m_scanLine);

    if (m_xpos >= m_screenBuffer->width()) return;
    if (m_ypos >= m_screenBuffer->height()) return;

    for (int pixel = 0; pixel < 8; ++pixel)
    {
        int ink1 = (displayByte1 & 0x80 ? 1 : 0);
        int ink2 = (displayByte2 & 0x80 ? 1 : 0);

        m_scanLine[m_xpos++] = m_inks[ink1];
        m_scanLine[(m_xpos+7)] = m_inks[ink2];

        displayByte1 <<= 1;
        displayByte2 <<= 1;
    }

    m_xpos += 8;
}
