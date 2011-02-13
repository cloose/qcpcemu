#ifndef Z80_TABLES_H
#define Z80_TABLES_H

#include "types.h"


static byte_t SignAndZeroTable[256] =
    {
        Z_FLAG,      0,      0,      0,      0,      0,      0,      0,
             0,      0,      0,      0,      0,      0,      0,      0,
             0,      0,      0,      0,      0,      0,      0,      0,
             0,      0,      0,      0,      0,      0,      0,      0,
             0,      0,      0,      0,      0,      0,      0,      0,
             0,      0,      0,      0,      0,      0,      0,      0,
             0,      0,      0,      0,      0,      0,      0,      0,
             0,      0,      0,      0,      0,      0,      0,      0,
             0,      0,      0,      0,      0,      0,      0,      0,
             0,      0,      0,      0,      0,      0,      0,      0,
             0,      0,      0,      0,      0,      0,      0,      0,
             0,      0,      0,      0,      0,      0,      0,      0,
             0,      0,      0,      0,      0,      0,      0,      0,
             0,      0,      0,      0,      0,      0,      0,      0,
             0,      0,      0,      0,      0,      0,      0,      0,
             0,      0,      0,      0,      0,      0,      0,      0,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG,
        S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG, S_FLAG
    };

static byte_t ParityTable[256] =
    {
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG,
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG,
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG,
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG,
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG,
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG,
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG,
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG,
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG,
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG,
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
        P_FLAG,      0,      0, P_FLAG,      0, P_FLAG, P_FLAG,      0,
             0, P_FLAG, P_FLAG,      0, P_FLAG,      0,      0, P_FLAG
    };

#endif // Z80_TABLES_H
