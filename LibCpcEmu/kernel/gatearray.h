#ifndef GATEARRAY_H
#define GATEARRAY_H

#include <QtCore/qobject.h>
#include "ioport.h"

class ScreenRenderer;
class VideoController;
class Z80;


/**
 * \brief A class representing the gate array in the Amstrad CPC
 */
class GateArray : public QObject, public IoPort
{
    Q_OBJECT

public:
    GateArray(Z80* cpu, VideoController* crtc, QObject* parent = 0);

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
    int    m_frameCycleCount;

    Z80* m_cpu;
    VideoController* m_crtc;
    ScreenRenderer* m_renderer;
};

#endif // GATEARRAY_H
