#ifndef Z80_REGISTERS_H
#define Z80_REGISTERS_H

#include "types.h"

namespace Z80 {

union RegisterSet
{
    // Word registers.
    struct
    {
        word_t AF, BC, DE, HL, IX, IY, SP;
    } wr;

    // Byte registers.
    struct
    {
        byte_t F, A, C, B, E, D, L, H, IXl, IXh, IYl, IYh;
    } br;
};


struct Register
{
    static RegisterSet R1;
    static RegisterSet R2;

    static word_t PC;               // 16-bit Program Counter register

    static byte_t I;                // 8-bit Interrupt Vector
    static byte_t R;                // 8-bit Memory Refresh

    /* interrupt flip-flops */
    static byte_t IFF1;             // IFF1: used to disable interrupts
    static byte_t IFF2;             // IFF2: temporary storage for IFF1
};

#define WR (Register::R1.wr)
#define BR (Register::R1.br)

} // namespace Z80

#endif // Z80_REGISTERS_H
