#ifndef IOCONTROLLER_H
#define IOCONTROLLER_H

#include <QtCore/qobject.h>
#include "ioport.h"

class Keyboard;

// LK1, LK2 and LK3 connected to Bits 1-3 of port B
enum ManufacturerIds
{
    ISP = 0,            // 0000 0000
    TRIUMPH = 2,        // 0000 0010
    SAISHO = 4,         // 0000 0100
    SOLAVOX = 6,        // 0000 0110
    AWA = 8,            // 0000 1000
    SCHNEIDER = 10,     // 0000 1010
    ORION = 12,         // 0000 1100
    AMSTRAD = 14        // 0000 1110
};

// LK4 connected to Bit 4 of port B
enum ScreenRefreshRates
{
    Refresh60Hz = 0,    // 60 Hz (PAL)
    Refresh50Hz = 1     // 50 Hz (SECAM)
};


/**
 * \brief The 8255 PPI chip in a Amstrad CPC system
 */
class IoController : public QObject, public IoPort
{
    Q_OBJECT

public:
    explicit IoController(Keyboard* keyboard, QObject* parent = 0);

    virtual bool in(word_t address, byte_t& value);
    virtual bool out(word_t address, byte_t value);

public slots:
    void vSync(bool active);

private:
    byte_t m_portA;
    byte_t m_portB;
    byte_t m_portC;
    byte_t m_control;
    bool m_vsyncActive;
    Keyboard* m_keyboard;
};

#endif // IOCONTROLLER_H
