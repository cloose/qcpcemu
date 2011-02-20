#ifndef VIDEOCONTROLLER_H
#define VIDEOCONTROLLER_H

#include "ioport.h"

class VideoControllerPrivate;


/**
 * \brief The CRTC 6845 chip in a Amstrad CPC system
 */
class VideoController : public IoPort
{
public:
    VideoController();
    virtual ~VideoController();

    void run(int cycles);

    virtual bool in(word_t address, byte_t& value);
    virtual bool out(word_t address, byte_t value);

private:
    byte_t readRegisterValue();
    void writeRegisterValue(byte_t value);

    VideoControllerPrivate* const d;
};

#endif // VIDEOCONTROLLER_H
