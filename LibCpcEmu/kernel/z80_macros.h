#ifndef Z80_MACROS_H
#define Z80_MACROS_H

#include "types.h"


#define S_FLAG 0x80     // sign flag      (1: result negative)
#define Z_FLAG 0x40     // zero flag      (1: Result is zero)
#define H_FLAG 0x10     // halfcarry flag (1: Halfcarry/Halfborrow)
#define P_FLAG 0x04     // parity flag    (1: Result is even)
#define V_FLAG 0x04     // overflow flag  (1: Overflow occured)
#define N_FLAG 0x02     // negative flag  (1: Subtraction occured)
#define C_FLAG 0x01     // carry flag     (1: Carry/Borrow occured)

//----------------------------------------------------------------------------
//- 8-Bit Load Group
//----------------------------------------------------------------------------

#define LD8_TO_REG(reg) \
{ \
    reg = readByteFromMemory(REGISTER_PC++); \
}

//----------------------------------------------------------------------------
//- 16-Bit Load Group
//----------------------------------------------------------------------------

#define LD16_TO_REG(reg) \
{ \
    reg.low = readByteFromMemory(REGISTER_PC++); \
    reg.high = readByteFromMemory(REGISTER_PC++); \
}

//----------------------------------------------------------------------------
//- 8-Bit Arithmetic Group
//----------------------------------------------------------------------------


// Condition Bits Affected:
// S is set if result is negative; reset otherwise
// Z is set if result is zero; reset otherwise
// H is set if carry from bit 3; reset otherwise
// P/V is set if r was 0x7f before operation; reset otherwise
// N is reset
// C is not affected
#define Inc(reg) \
{ \
    reg++; \
    REGISTER_F = (REGISTER_F & C_FLAG) \
               | SignAndZeroTable[reg] \
               | (reg == 0x80 ? V_FLAG : 0) \
               | (reg &  0x0f ? 0 : H_FLAG); \
}

// Condition Bits Affected:
// S is set if result is negative; reset otherwise
// Z is set if result is zero; reset otherwise
// H is set if borrow from bit 4; reset otherwise
// P/V is set if r was 0x80 before operation; reset otherwise
// N is set
// C is not affected
#define Dec(reg) \
{ \
    reg--; \
    REGISTER_F = N_FLAG \
               | (REGISTER_F & C_FLAG) \
               | SignAndZeroTable[reg] \
               | (reg == 0x7f ? V_FLAG : 0) \
               | ((reg &  0x0f) == 0x0f ? H_FLAG : 0); \
}

//----------------------------------------------------------------------------
//- Jump Group
//----------------------------------------------------------------------------

#define JUMP \
{ \
    REGISTER_PC = readWordFromMemory(REGISTER_PC); \
}

#endif // Z80_MACROS_H
