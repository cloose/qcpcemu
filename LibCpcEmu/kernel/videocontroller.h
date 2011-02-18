#ifndef VIDEOCONTROLLER_H
#define VIDEOCONTROLLER_H

#include "ioport.h"


/**
 * \brief The CRTC 6845 chip in a Amstrad CPC system
 */
class VideoController : public IoPort
{
public:
    VideoController();

    void run(int cycles);

    virtual bool in(word_t address, byte_t& value);
    virtual bool out(word_t address, byte_t value);

private:
    quint8 m_currentRegister;
    byte_t m_registers[18];
};

#endif // VIDEOCONTROLLER_H
