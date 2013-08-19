#ifndef GATEARRAY_H
#define GATEARRAY_H

#include <QtCore/qobject.h>
#include "ioport.h"

class ScreenRenderer;
class SoundGenerator;
class VideoController;

namespace Z80 {
class Cpu;
}

/**
 * \brief A class representing the gate array in the Amstrad CPC
 */
class GateArray : public QObject, public IoPort
{
    Q_OBJECT

public:
    GateArray(Z80::Cpu* cpu, VideoController* crtc, SoundGenerator* psg, QObject* parent = 0);

    void run();

    void setRenderer(ScreenRenderer* renderer);

    virtual bool in(word_t address, byte_t& value);
    virtual bool out(word_t address, byte_t value);

public slots:
    void hSync(bool active);
    void vSync(bool active);
    void endOfFrame();

private:
    void selectPen(byte_t value);
    void selectColorForPen(byte_t value);
    void setRomConfiguration(byte_t value);
    void selectUpperRom(quint8 romNumber);

    byte_t m_inkValues[17];
    byte_t m_currentPen;
    byte_t m_scanlineCounter;
    byte_t m_screenMode;
    bool   m_screenModeChanged;
    int    m_frameCycleCount;
    byte_t m_upperRomNumber;

    Z80::Cpu* m_cpu;
    VideoController* m_crtc;
    SoundGenerator* m_psg;
    ScreenRenderer* m_renderer;
};

#endif // GATEARRAY_H
