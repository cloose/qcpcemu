#ifndef REGISTERSET_H
#define REGISTERSET_H

#include"types.h"


struct RegisterSet
{
    /* main register set */
    static register_pair_t AF,
                           BC,
                           DE,
                           HL,
                           PC,  // 16-bit Program Counter register
                           SP;  // 16-bit Stack Pointer register
};


// convenience macros to access cpu registers
#define REGISTER_PC RegisterSet::PC.pair

#endif // REGISTERSET_H
