#include "types.h"
#include "memory.h"
#include "registerset.h"
#include "z80_tables.h"

//----------------------------------------------------------------------------
//- Memory Access
//----------------------------------------------------------------------------

static inline byte_t ReadByteFromMemory(word_t address)
{
    quint8 block = (address >> 14);
    quint16 addressOffset = block * 0x4000;

    return Memory::blocks[block][address - addressOffset];
}

static inline word_t ReadWordFromMemory(word_t address)
{
    byte_t low = ReadByteFromMemory(address++);
    byte_t high = ReadByteFromMemory(address);

    return (low | (high << 8));
}

static inline void WriteByteToMemory(word_t address, byte_t value)
{
    Memory::ram[address] = value;
}


//----------------------------------------------------------------------------
//- 8-Bit Load Group
//----------------------------------------------------------------------------

/**
 * The 8-bit integer n is loaded to any register r, where r identifies register A,
 * B, C, D, E, H, or L.
 *
 * Condition Bits Affected: None
 */
static inline void LoadByteToReg(byte_t& reg)
{
    reg = ReadByteFromMemory(REGISTER_PC++);
}

/**
 * The contents of the Accumulator are loaded to the memory address specified by the
 * operand nn.
 *
 * Condition Bits Affected: None
 */
static inline void LoadAccumulatorToMem(word_t nn)
{
    Memory::ram[nn] = REGISTER_A;
}

/**
 * The contents of the memory location specified by the operands nn are loaded to
 * the Accumulator.
 *
 * Condition Bits Affected: None
 */
static inline void LoadAccumulatorFromMem(word_t nn)
{
    REGISTER_A = ReadByteFromMemory(nn);
}


//----------------------------------------------------------------------------
//- 16-Bit Load Group
//----------------------------------------------------------------------------

/**
 * The 2-byte integer nn is loaded to the dd register pair, where dd defines the
 * BC, DE, HL, or SP register pairs.
 *
 * Condition Bits Affected: None
 */
static inline void LoadWordToReg(register_pair_t& reg)
{
    reg.low = ReadByteFromMemory(REGISTER_PC++);
    reg.high = ReadByteFromMemory(REGISTER_PC++);
}


//----------------------------------------------------------------------------
//- Exchange, Block Transfer, and Search Group
//----------------------------------------------------------------------------

/**
 * Each 2-byte value in register pairs BC, DE, and HL is exchanged with the
 * 2-byte value in BC', DE', and HL', respectively.
 *
 * Condition Bits Affected: None
 */
static inline void Exx()
{
    qSwap(REGISTER_BC, REGISTER_BC1);
    qSwap(REGISTER_DE, REGISTER_DE1);
    qSwap(REGISTER_HL, REGISTER_HL1);
}

/**
 * A byte of data is transferred from the memory location addressed, by the contents of
 * the HL register pair to the memory location addressed by the contents of the
 * DE register pair. Then both these register pairs are incremented and the BC
 * (Byte Counter) register pair is decremented.
 *
 * Condition Bits Affected:
 *     S is not affected
 *     Z is not affected
 *     H is reset
 *     P/V is set if BC -1 != 0; reset otherwise
 *     N is reset
 *     C is not affected
 */
static inline void Ldi()
{
    // transfer data from (HL) to (DE)
    WriteByteToMemory(REGISTER_DE, ReadByteFromMemory(REGISTER_HL));

    REGISTER_F &= (S_FLAG | Z_FLAG | C_FLAG);

    REGISTER_HL++;
    REGISTER_DE++;
    REGISTER_BC--;

    // set overflow flag if BC not equal 0
    if (REGISTER_BC != 0)
        REGISTER_F |= V_FLAG;
}


//----------------------------------------------------------------------------
//- 8-Bit Arithmetic Group
//----------------------------------------------------------------------------

/**
 * A logical AND operation is performed between the byte specified by the s operand
 * and the byte contained in the Accumulator; the result is stored in the Accumulator.
 * The s operand is any of r, n, (HL), (IX+d), or (IY+d).
 *
 * Condition Bits Affected:
 *     S is set if result is negative; reset otherwise
 *     Z is set if result is zero; reset otherwise
 *     H is set
 *     P/V is set if overflow; reset otherwise
 *     N is reset
 *     C is reset
 */
static inline void And(byte_t s)
{
    REGISTER_A &= s;

    REGISTER_F  = H_FLAG
                | ParityTable[REGISTER_A]
                | SignAndZeroTable[REGISTER_A];
}


/**
 * A logical OR operation is performed between the byte specified by the s operand
 * and the byte contained in the Accumulator; the result is stored in the Accumulator.
 * The s operand is any of r, n, (HL), (IX+d), or (IY+d).
 *
 * Condition Bits Affected:
 *     S is set if result is negative; reset otherwise
 *     Z is set if result is zero; reset otherwise
 *     H is reset
 *     P/V is set if overflow; reset otherwise
 *     N is reset
 *     C is reset
 */
static inline void Or(byte_t s)
{
    REGISTER_A |= s;

    REGISTER_F  = SignAndZeroTable[REGISTER_A]
                | ParityTable[REGISTER_A];
}

/**
 * Register r is incremented and register r identifies any of the registers A, B,
 * C, D, E, H, or L.
 *
 * Condition Bits Affected:
 *     S is set if result is negative; reset otherwise
 *     Z is set if result is zero; reset otherwise
 *     H is set if carry from bit 3; reset otherwise
 *     P/V is set if r was 0x7f before operation; reset otherwise
 *     N is reset
 *     C is not affected
 */
static inline void Inc(byte_t& r)
{
    r++;

    REGISTER_F = (REGISTER_F & C_FLAG)
               | SignAndZeroTable[r]
               | (r == 0x80 ? V_FLAG : 0)
               | (r &  0x0f ? 0 : H_FLAG);
}

/**
 * The byte specified by the m operand is decremented. The m operand is any of r, (HL),
 * (IX+d), or (IY+d),
 *
 * Condition Bits Affected:
 *     S is set if result is negative; reset otherwise
 *     Z is set if result is zero; reset otherwise
 *     H is set if borrow from bit 4; reset otherwise
 *     P/V is set if r was 0x80 before operation; reset otherwise
 *     N is set
 *     C is not affected
 */
static inline void Dec(byte_t& m)
{
    m--;

    REGISTER_F = N_FLAG
               | (REGISTER_F & C_FLAG)
               | SignAndZeroTable[m]
               | (m == 0x7f ? V_FLAG : 0)
               | ((m &  0x0f) == 0x0f ? H_FLAG : 0);
}


//----------------------------------------------------------------------------
//- 16-Bit Arithmetic Group
//----------------------------------------------------------------------------

/**
 * The contents of register pair ss (any of register pairs BC, DE, HL, or SP)
 * are incremented.
 *
 * Condition Bits Affected: None
 */
static inline void Inc(word_t& ss)
{
    ss++;
}

/**
 * The contents of register pair ss (any of register pairs BC, DE, HL, or SP)
 * are decremented.
 *
 * Condition Bits Affected: None
 */
static inline void Dec(word_t& ss)
{
    ss--;
}


//----------------------------------------------------------------------------
//- Jump Group
//----------------------------------------------------------------------------

/**
 * Operand nn is loaded to register pair PC (Program Counter). The next
 * instruction is fetched from the location designated by the new contents of
 * the PC.
 *
 * Condition Bits Affected: None
 */
static inline void Jump()
{
    REGISTER_PC = ReadWordFromMemory(REGISTER_PC);
}

/**
 * The value of the displacement e is added to the Program Counter (PC) and the next
 * instruction is fetched from the location designated by the new contents of the PC.
 * This jump is measured from the address of the instruction Op Code and has a range
 * of -126 to +129 bytes. The assembler automatically adjusts for the twice
 * incremented PC.
 *
 * Condition Bits Affected: None
 */
static inline void JumpRelative()
{
    REGISTER_PC += static_cast<offset_t>(ReadByteFromMemory(REGISTER_PC)) + 1;
}
