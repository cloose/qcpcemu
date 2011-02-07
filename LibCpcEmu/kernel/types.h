#ifndef TYPES_H
#define TYPES_H

#include <QtCore/qglobal.h>

typedef quint8 byte_t;
typedef quint16 word_t;

union register_pair_t
{
    struct
    {
        byte_t low;
        byte_t high;
    };

    word_t pair;
};

#endif // TYPES_H
