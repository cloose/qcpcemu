#ifndef CPCSYSTEM_H
#define CPCSYSTEM_H

#include "LibCpcEmu_global.h"
#include <QtCore/qobject.h>
#include "types.h"

class CpcSystemPrivate;
class FloppyDiskDrive;
class Keyboard;
class ScreenRenderer;


class LIBCPCEMUSHARED_EXPORT CpcSystem : public QObject
{
    Q_OBJECT

public:
    CpcSystem();
    ~CpcSystem();

    void run();
    void step();

    Keyboard* keyboard() const;

    void attachDiskDrive(uint number, FloppyDiskDrive* drive);
    void setRenderer(ScreenRenderer* renderer);
    void loadExternalRom(quint8 romNumber, const QString& fileName);

    void addBreakpoint(word_t address);

public slots:
    void stopSystem();

private:
    CpcSystemPrivate* const d;
};

#endif // CPCSYSTEM_H
