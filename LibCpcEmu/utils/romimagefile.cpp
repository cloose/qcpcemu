#include "romimagefile.h"

#include <QFile>


RomImageFile::RomImageFile(const QString& name)
    : m_fileName(name)
{
}

RomImageFile::~RomImageFile()
{
}

bool RomImageFile::load()
{
    QFile imageFile(m_fileName);

    if (!imageFile.open(QIODevice::ReadOnly))
        return false;

    m_data = imageFile.readAll();

    return true;
}

int RomImageFile::size() const
{
    return m_data.size();
}

const byte_t* RomImageFile::constData() const
{
    return reinterpret_cast<const byte_t*>(m_data.constData());
}
