#ifndef Z80_MACROS_H
#define Z80_MACROS_H

#define LD16_TO_REG(reg) \
{ \
    reg.low = readByteFromMemory(REGISTER_PC++); \
    reg.high = readByteFromMemory(REGISTER_PC++); \
}

#endif // Z80_MACROS_H
