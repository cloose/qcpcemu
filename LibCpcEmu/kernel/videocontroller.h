#ifndef VIDEOCONTROLLER_H
#define VIDEOCONTROLLER_H

#include <QtCore/qobject.h>
#include "ioport.h"

class VideoControllerPrivate;


/**
 * \brief The CRTC 6845 chip in a Amstrad CPC system
 */
class VideoController : public QObject, public IoPort
{
    Q_OBJECT

public:
    explicit VideoController(QObject* parent = 0);
    virtual ~VideoController();

    void run();
    void run(int cycles);

    virtual bool in(word_t address, byte_t& value);
    virtual bool out(word_t address, byte_t value);

    word_t memoryAddress() const;
    byte_t rowAddress() const;

signals:
    void hSync(bool active);
    void vSync(bool active);

private:
    byte_t readRegisterValue();
    void writeRegisterValue(byte_t value);

    void activateHSyncSignal();
    void deactivateHSyncSignal();
    void activateVSyncSignal();
    void deactivateVSyncSignal();

    VideoControllerPrivate* const d;
    friend class VideoControllerPrivate;
};

#endif // VIDEOCONTROLLER_H
