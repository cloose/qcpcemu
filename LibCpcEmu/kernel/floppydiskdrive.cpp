#include "floppydiskdrive.h"

#include "diskimagefile.h"


FloppyDiskDrive::FloppyDiskDrive()
    : m_diskImage(0)
{
}

FloppyDiskDrive::~FloppyDiskDrive()
{
    delete m_diskImage;
}

void FloppyDiskDrive::insertDisk(const QString& fileName)
{
    // eject current disk
    ejectDisk();

    // open disk image file
    m_diskImage = new DiskImageFile(fileName);
    m_diskImage->open();
}

void FloppyDiskDrive::ejectDisk()
{
    delete m_diskImage;
    m_diskImage = 0;
}
