#ifndef REGISTERSET_H
#define REGISTERSET_H

#include"types.h"


struct RegisterSet
{
    /* main register set */
    static register_pair_t AF,  // 16-bit register pair containing A and flags
                           BC,  // 16-bit register pair containing B and C
                           DE,  // 16-bit register pair containing D and E
                           HL,  // 16-bit register pair containing H and L
                           PC,  // 16-bit Program Counter register
                           SP;  // 16-bit Stack Pointer register

    /* shadow register set */
    static register_pair_t AF1, // 16-bit register pair containing A and flags
                           BC1, // 16-bit register pair containing B and C
                           DE1, // 16-bit register pair containing D and E
                           HL1; // 16-bit register pair containing H and L

    /* interrupt flip-flops */
    static byte_t IFF1,         // IFF1: used to disable interrupts
                  IFF2;         // IFF2: temporary storage for IFF1
};


// convenience macros to access cpu registers
#define REGISTER_AF RegisterSet::AF.pair
#define REGISTER_BC RegisterSet::BC.pair
#define REGISTER_DE RegisterSet::DE.pair
#define REGISTER_HL RegisterSet::HL.pair
#define REGISTER_PC RegisterSet::PC.pair
#define REGISTER_SP RegisterSet::SP.pair

#define REGISTER_A   RegisterSet::AF.high
#define REGISTER_F   RegisterSet::AF.low
#define REGISTER_B   RegisterSet::BC.high
#define REGISTER_C   RegisterSet::BC.low
#define REGISTER_D   RegisterSet::DE.high
#define REGISTER_E   RegisterSet::DE.low
#define REGISTER_H   RegisterSet::HL.high
#define REGISTER_L   RegisterSet::HL.low

#define REGISTER_AF1 RegisterSet::AF1.pair
#define REGISTER_BC1 RegisterSet::BC1.pair
#define REGISTER_DE1 RegisterSet::DE1.pair
#define REGISTER_HL1 RegisterSet::HL1.pair

#endif // REGISTERSET_H
