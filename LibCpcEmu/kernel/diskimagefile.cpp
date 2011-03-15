#include "diskimagefile.h"

#include <QDataStream>
#include <QFile>


DiskImageFile::DiskImageFile(const QString& fileName)
    : m_fileName(fileName)
    , m_header(0)
    , m_tracks(0)
{
}

DiskImageFile::~DiskImageFile()
{
    for (int i = 0; i < m_header->numberOfTracks*m_header->numberOfSides; ++i)
    {
        delete[] m_tracks[i].data;
        delete[] m_tracks[i].header->sectorInfos;
        delete m_tracks[i].header;
    }
    delete[] m_tracks;
    delete m_header;
}

bool DiskImageFile::open()
{
    QFile imageFile(m_fileName);
    if (!imageFile.open(QIODevice::ReadOnly))
    {
        return false;
    }

    QDataStream in(&imageFile);

    // read disk information block
    m_header = readDiskInformationBlock(in);

    // read tracks
    m_tracks = new Track[m_header->numberOfTracks*m_header->numberOfSides];
    for (int i = 0; i < m_header->numberOfTracks*m_header->numberOfSides; ++i)
    {
        qint64 start = imageFile.pos();
        m_tracks[i].header = readTrackInformationBlock(in);

        // sector data follows at offset 0x100 from the start of the track info block.
        qint64 skip = (start + 0x100) - imageFile.pos();
        in.skipRawData(skip);

        // read sector data
        qint64 size = m_tracks[i].header->sectorSize * 0x100 * m_tracks[i].header->numberOfSectors;
        m_tracks[i].data = new byte_t[size];
        for (int j = 0; j < size; ++j)
        {
            in >> m_tracks[i].data[j];
        }
    }

    return true;
}

bool DiskImageFile::isExtendedFormat() const
{
    QString tag = QString::fromLatin1(reinterpret_cast<const char*>(m_header->tag), 34);
    return tag.startsWith("EXTENDED CPC DSK File");
}

DiskInformationBlock* DiskImageFile::readDiskInformationBlock(QDataStream& in)
{
    DiskInformationBlock* diskInfo = new DiskInformationBlock;

    // tag
    for (int i = 0; i < 34; ++i)
    {
        in >> diskInfo->tag[i];
    }

    // TODO: check if this is really a DSK image file

    // name of creator
    for (int i = 0; i < 14; ++i)
    {
        in >> diskInfo->nameOfCreator[14];
    }

    in >> diskInfo->numberOfTracks;
    in >> diskInfo->numberOfSides;

    if (isExtendedFormat())
    {
        in >> diskInfo->ExtendedFormat.unusedExt[0];
        in >> diskInfo->ExtendedFormat.unusedExt[1];

        for (int i = 0; i < 204; ++i)
        {
            in >> diskInfo->ExtendedFormat.trackSizeTable[i];
        }
    }
    else
    {
        in >> diskInfo->StandardFormat.trackSizeLow;
        in >> diskInfo->StandardFormat.trackSizeHigh;

        for (int i = 0; i < 204; ++i)
        {
            in >> diskInfo->StandardFormat.unusedStd[0];
        }
    }

    return diskInfo;
}

TrackInformationBlock* DiskImageFile::readTrackInformationBlock(QDataStream &in)
{
    TrackInformationBlock* trackInfo = new TrackInformationBlock;

    // tag
    for (int i = 0; i < 12; ++i)
    {
        in >> trackInfo->tag[i];
    }

    // 4 unused bytes
    byte_t unused;
    in >> unused >> unused >> unused >> unused;

    in >> trackInfo->trackNumber;
    in >> trackInfo->sideNumber;

    // 2 unused bytes
    in >> unused >> unused;

    in >> trackInfo->sectorSize;
    in >> trackInfo->numberOfSectors;
    in >> trackInfo->gap3Length;
    in >> trackInfo->filler;

    // read valid sector info blocks
    trackInfo->sectorInfos = new SectorInfo[trackInfo->numberOfSectors];
    for (int i = 0; i < trackInfo->numberOfSectors; ++i)
    {
        in >> trackInfo->sectorInfos[i].track;
        in >> trackInfo->sectorInfos[i].side;
        in >> trackInfo->sectorInfos[i].sectorId;
        in >> trackInfo->sectorInfos[i].sectorSize;
        in >> trackInfo->sectorInfos[i].st1;
        in >> trackInfo->sectorInfos[i].st2;
        in >> trackInfo->sectorInfos[i].dataLength;
    }

    return trackInfo;
}
