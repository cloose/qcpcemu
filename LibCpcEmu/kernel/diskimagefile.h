#ifndef DISKIMAGEFILE_H
#define DISKIMAGEFILE_H

#include <QtCore/qstring.h>
#include "types.h"

class QDataStream;

struct DiskInformationBlock
{
    byte_t tag[34];             // 0x00-0x21
    byte_t nameOfCreator[14];   // 0x22-0x2f
    byte_t numberOfTracks;      // 0x30
    byte_t numberOfSides;       // 0x31

    union
    {
        // standard format
        struct
        {
            byte_t trackSizeLow;        // 0x32
            byte_t trackSizeHigh;       // 0x33
            byte_t unusedStd[204];      // 0x34-0xff
        } StandardFormat;

        // extended format
        struct
        {
            byte_t unusedExt[2];        // 0x32-0x33
            byte_t trackSizeTable[204]; // 0x34-0xff
        } ExtendedFormat;
    };
};

struct SectorInfo
{
    byte_t track;               // 0x00 - equivalent to C parameter
    byte_t side;                // 0x01 - equivalent to H parameter
    byte_t sectorId;            // 0x02 - equivalent to R parameter
    byte_t sectorSize;          // 0x03 - equivalent to N parameter
    byte_t st1;                 // 0x04
    byte_t st2;                 // 0x05
    quint16 dataLength;         // 0x06-0x07
};

struct TrackInformationBlock
{
    byte_t      tag[12];             // 0x00-0x0b
                                     // 0x0c-0x0f
    byte_t      trackNumber;         // 0x10
    byte_t      sideNumber;          // 0x11
                                     // 0x12-0x13
    byte_t      sectorSize;          // 0x14
    byte_t      numberOfSectors;     // 0x15
    byte_t      gap3Length;          // 0x16
    byte_t      filler;              // 0x17
    SectorInfo* sectorInfos;         // 0x18-xx
};

struct Track
{
    TrackInformationBlock* header;
    byte_t*                data;
};


class DiskImageFile
{
public:
    explicit DiskImageFile(const QString& fileName);
    ~DiskImageFile();

    bool open();

    bool isExtendedFormat() const;

private:
    Q_DISABLE_COPY(DiskImageFile);

    DiskInformationBlock* readDiskInformationBlock(QDataStream& in);
    TrackInformationBlock* readTrackInformationBlock(QDataStream& in);

    QString m_fileName;
    DiskInformationBlock* m_header;
    Track* m_tracks;
};

#endif // DISKIMAGEFILE_H
