#ifndef SCREENRENDERER_H
#define SCREENRENDERER_H

#include "types.h"


class ScreenRenderer
{
public:
    virtual void draw(byte_t displayByte1, byte_t displayByte2) = 0;

    virtual void hSync() = 0;
    virtual void vSync(bool active) = 0;

    virtual void setColor(uchar penNum, uchar colorNum) = 0;
};

#endif // SCREENRENDERER_H
