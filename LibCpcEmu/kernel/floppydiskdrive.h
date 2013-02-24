#ifndef FLOPPYDISKDRIVE_H
#define FLOPPYDISKDRIVE_H

#include "LibCpcEmu_global.h"
#include "types.h"

class DiskImageFile;
class QString;
struct SectorInfo;


class LIBCPCEMUSHARED_EXPORT FloppyDiskDrive
{
public:
    FloppyDiskDrive();
    ~FloppyDiskDrive();

    void insertDisk(const QString& fileName);
    void ejectDisk();
    bool hasDisk() const;

    int findSector(byte_t sectorId);
    byte_t readData(uint position);
    SectorInfo* readSectorInfo();
    void seek(uint track);

    byte_t side() const { return m_side; }
    void setSide(byte_t side) { m_side = side; }
    byte_t track() const { return m_track; }

private:
    byte_t         m_side;        // current side number
    byte_t         m_track;       // current track number
    byte_t         m_sector;      // current sector number

    DiskImageFile* m_diskImage;
};

#endif // FLOPPYDISKDRIVE_H
