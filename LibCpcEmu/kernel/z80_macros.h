#ifndef Z80_MACROS_H
#define Z80_MACROS_H

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
//- Jump Group
//----------------------------------------------------------------------------

#define JUMP \
{ \
    REGISTER_PC = readWordFromMemory(REGISTER_PC); \
}

#endif // Z80_MACROS_H
