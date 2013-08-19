#ifndef QIMAGESCREENRENDERER_H
#define QIMAGESCREENRENDERER_H

#include "screenrenderer.h"
#include <QImage>

//class QImage;
class QWidget;


class QImageScreenRenderer : public ScreenRenderer
{
public:
    explicit QImageScreenRenderer(QWidget* widget);
    virtual ~QImageScreenRenderer();

    const QImage screenBuffer() const;

    virtual void draw(byte_t displayByte1, byte_t displayByte2);

    virtual void hSync();
    virtual void vSync(bool active);

    virtual void setColor(uchar penNum, uchar colorNum);
    virtual QColor borderColor() const;

    virtual void setMode(byte_t mode);

    long frameCounter() const { return m_frameCounter; }

private:
    void drawMode0(byte_t displayByte1, byte_t displayByte2);
    void drawMode1(byte_t displayByte1, byte_t displayByte2);
    void drawMode2(byte_t displayByte1, byte_t displayByte2);

    typedef void (QImageScreenRenderer::*DrawFunction)(byte_t, byte_t);

    QWidget* m_widget;

    uint m_xpos, m_ypos;
    uchar m_inks[17];

    QImage* m_screenBuffer;
//    uchar* m_scanLine;
    QRgb* m_scanLine;
    DrawFunction m_drawFuncPtr;

    long m_frameCounter;
};

#endif // QIMAGESCREENRENDERER_H
