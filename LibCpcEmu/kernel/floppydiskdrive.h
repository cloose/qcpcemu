#ifndef FLOPPYDISKDRIVE_H
#define FLOPPYDISKDRIVE_H

#include "LibCpcEmu_global.h"

class DiskImageFile;
class QString;


class LIBCPCEMUSHARED_EXPORT FloppyDiskDrive
{
public:
    FloppyDiskDrive();
    ~FloppyDiskDrive();

    void insertDisk(const QString& fileName);
    void ejectDisk();

private:
    DiskImageFile* m_diskImage;
};

#endif // FLOPPYDISKDRIVE_H
