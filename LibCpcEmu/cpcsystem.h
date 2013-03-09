#ifndef CPCSYSTEM_H
#define CPCSYSTEM_H

#include "LibCpcEmu_global.h"
#include <QtCore/qobject.h>
#include "types.h"

class CpcSystemPrivate;
class FloppyDiskDrive;
class Keyboard;
class ScreenRenderer;
class QIODevice;


class LIBCPCEMUSHARED_EXPORT CpcSystem : public QObject
{
    Q_OBJECT

public:
    explicit CpcSystem(QObject* parent = 0);
    ~CpcSystem();

    void run();
    void step();
    void stop();
    void reset();

    Keyboard* keyboard() const;

    void attachDiskDrive(uint number, FloppyDiskDrive* drive);
    void setRenderer(ScreenRenderer* renderer);
    void setAudioDevice(QIODevice *device);
    void loadExternalRom(quint8 romNumber, const QString& fileName);

    void addBreakpoint(word_t address);

private:
    const QScopedPointer<CpcSystemPrivate> d;
};

#endif // CPCSYSTEM_H
