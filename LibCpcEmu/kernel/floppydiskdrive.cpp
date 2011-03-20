#include "floppydiskdrive.h"

#include "diskimagefile.h"


FloppyDiskDrive::FloppyDiskDrive()
    : m_side(0)
    , m_track(0)
    , m_sector(0)
    , m_diskImage(0)
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

bool FloppyDiskDrive::hasDisk() const
{
    return (m_diskImage != 0);
}

int FloppyDiskDrive::findSector(byte_t sectorId)
{
    return m_diskImage->sector(m_track, sectorId);
}

byte_t FloppyDiskDrive::readData(uint position)
{
    uint trackIndex = m_track * m_diskImage->header()->numberOfSides + m_side;
    return m_diskImage->data(trackIndex, position);
}

SectorInfo* FloppyDiskDrive::readSectorInfo()
{
    SectorInfo* sectorInfo = m_diskImage->sectorInfo(m_track, m_sector);
    m_sector++;
    return sectorInfo;
}

void FloppyDiskDrive::seek(uint track)
{
    // seek beyond track maximum?
    if (track >= m_diskImage->header()->numberOfTracks)
        track = m_diskImage->header()->numberOfTracks-1;

    m_track  = track;
    m_sector = 0;
}
