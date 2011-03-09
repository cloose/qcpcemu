#ifndef ROMIMAGEFILE_H
#define ROMIMAGEFILE_H

#include "types.h"
#include <QtCore/qstring.h>


class RomImageFile
{
public:
    explicit RomImageFile(const QString& name);
    ~RomImageFile();

    bool load();

    int size() const;
    const byte_t* constData() const;

private:
    QString    m_fileName;
    QByteArray m_data;
};

#endif // ROMIMAGEFILE_H
