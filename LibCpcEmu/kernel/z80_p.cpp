#include "types.h"
#include "memory.h"
#include "registerset.h"
#include "z80_tables.h"

// high order byte of a word
#define HIBYTE(word)       ((word >> 8) & 0xff)

// low order byte of a word
#define LOBYTE(word)       (word & 0xff)

// make a word out of two bytes
#define WORD(low, high)     (low | (high << 8))


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

static inline void Load(byte_t& destination, byte_t source)
{
    destination = source;
}

static inline byte_t ConstantByte()
{
    return ReadByteFromMemory(REGISTER_PC++);
}

static inline byte_t& MemoryLocationW(word_t address)
{
    return Memory::ram[address];
}

static inline byte_t MemoryLocationR(word_t address)
{
    return ReadByteFromMemory(address);
}


//----------------------------------------------------------------------------
//- 16-Bit Load Group
//----------------------------------------------------------------------------

static inline void Load(word_t& destination, word_t source)
{
    destination = source;
}

static inline word_t ConstantWord()
{
    byte_t low = ReadByteFromMemory(REGISTER_PC++);
    byte_t high = ReadByteFromMemory(REGISTER_PC++);
    return WORD(low, high);
}

static inline word_t MemoryLocationWordR(word_t address)
{
    return ReadWordFromMemory(address);
}

/**
 * The contents of the register pair qq are pushed to the external memory LIFO
 * (last-in, first-out) Stack. The Stack Pointer (SP) register pair holds the 16-bit
 * address of the current top of the Stack. This instruction first decrements SP and
 * loads the high order byte of register pair qq to the memory address specified by
 * the SP. The SP is decremented again and loads the low order byte of qq to the
 * memory location corresponding to this new address in the SP. The operand qq
 * identifies register pair BC, DE, HL, or AF.
 *
 * Condition Bits Affected: None
 */
static inline void Push(word_t value)
{
    WriteByteToMemory(--REGISTER_SP, HIBYTE(value));
    WriteByteToMemory(--REGISTER_SP, LOBYTE(value));
}

/**
 * TODO: missing description!
 */
static inline word_t Pop()
{
    byte_t low  = ReadByteFromMemory(REGISTER_SP++);
    byte_t high = ReadByteFromMemory(REGISTER_SP++);

    // make sure Pop() and Push() is in balance
    Q_ASSERT(REGISTER_SP <= 0xc000);

    return WORD(low, high);
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
 * The s operand is subtracted from the contents of the Accumulator, and the
 * result is stored in the Accumulator.
 * The s operand is any of r, n, (HL), (IX+d), or (IY+d).
 *
 * Condition Bits Affected:
 *     S is set if result is negative; reset otherwise
 *     Z is set if result is zero; reset otherwise
 *     H is set if borrow from bit 4; reset otherwise
 *     P/V is set if overflow; reset otherwise
 *     N is set
 *     C is set if borrow; reset otherwise
 */
static inline void Sub(byte_t s)
{
    word_t result = REGISTER_A - s;

    REGISTER_F = ((REGISTER_A ^ s) & (REGISTER_A ^ LOBYTE(result)) & 0x80 ? V_FLAG : 0)
               | N_FLAG
               | -HIBYTE(result)
               | SignAndZeroTable[LOBYTE(result)]
               | ((REGISTER_A ^ s ^ LOBYTE(result)) & H_FLAG);

    REGISTER_A = LOBYTE(result);
}

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
 * A logical exclusive-OR operation is performed between the byte specified by the s operand
 * and the byte contained in the Accumulator; the result is stored in the Accumulator.
 * The s operand is any of r, n, (HL), (IX+d), or (IY+d).
 *
 * Condition Bits Affected:
 *     S is set if result is negative; reset otherwise
 *     Z is set if result is zero; reset otherwise
 *     H is reset
 *     P/V is set if parity even; reset otherwise
 *     N is reset
 *     C is reset
 */
static inline void Xor(byte_t value)
{
    REGISTER_A ^= value;

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
//- General-Purpose Arithmetic and CPU Control Groups
//----------------------------------------------------------------------------

/**
 * The contents of the Accumulator (register A) are inverted (one�s complement).
 *
 * Condition Bits Affected:
 *     S is not affected
 *     Z is not affected
 *     H is set
 *     P/V is not affected
 *     N is set
 *     C is not affected
 */
static inline void Cpl()
{
    REGISTER_A ^= 0xff;

    REGISTER_F = (REGISTER_F & (S_FLAG  | Z_FLAG | P_FLAG | C_FLAG))
               | H_FLAG
               | N_FLAG;
}


//----------------------------------------------------------------------------
//- 16-Bit Arithmetic Group
//----------------------------------------------------------------------------

/**
 * TODO: missing description
 *
 * Condition Bits Affected:
 *     S is not affected
 *     Z is not affected
 *     H is set if carry out of bit 11; reset otherwise
 *     P/V is not affected
 *     N is reset
 *     C is set if carry from bit 15; reset otherwise
 */
static inline void Add(word_t destination, word_t source)
{
    dword_t result = destination + source;

    REGISTER_F = (REGISTER_F & (S_FLAG | Z_FLAG | V_FLAG))
               | ((destination ^ result ^ source) & 0x1000 ? H_FLAG : 0)
               | (result & 0x10000 ? C_FLAG : 0);

    destination = (result & 0xffff);
}

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
//- Rotate and Shift Group
//----------------------------------------------------------------------------

/**
 * The contents of the Accumulator (register A) are rotated left 1-bit position.
 * The sign bit (bit 7) is copied to the Carry flag and also to bit 0. Bit 0 is the
 * least-significant bit.
 *
 * Condition Bits Affected:
 *     S is not affected
 *     Z is not affected
 *     H is reset
 *     P/V is not affected
 *     N is reset
 *     C is data from bit 7 of Accumulator
 */
static inline void Rlca()
{
    // bit 7 (sign bit) is the new carry flag
    byte_t cf = REGISTER_A & 0x80 ? C_FLAG : 0;

    // rotate left 1 bit and copy bit 7 to bit 0
    REGISTER_A = (REGISTER_A << 1) | (REGISTER_A >> 7);

    REGISTER_F = (REGISTER_F & (S_FLAG | Z_FLAG | P_FLAG))
               | cf;
}

/**
 * The contents of the Accumulator (register A) are rotated right 1-bit position.
 * Bit 0 is copied to the Carry flag and also to bit 7. Bit 0 is the least-significant
 * bit.
 *
 * Condition Bits Affected:
 *     S is not affected
 *     Z is not affected
 *     H is reset
 *     P/V is not affected
 *     N is reset
 *     C is data from bit 0 of Accumulator
 */
static inline void Rrca()
{
    // bit 0 (least-significant bit) is the new carry flag
    byte_t cf = REGISTER_A & 0x01 ? C_FLAG : 0;

    // rotate right 1 bit and copy bit 0 to bit 7
    REGISTER_A = (REGISTER_A >> 1) | (REGISTER_A << 7);

    REGISTER_F = (REGISTER_F & (S_FLAG | Z_FLAG | P_FLAG))
               | cf;
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


//----------------------------------------------------------------------------
//- Call and Return Group
//----------------------------------------------------------------------------

/**
 * TODO: description missing
 *
 * Condition Bits Affected: None
 */
static inline void Call()
{
    // retrieve address of subroutine
    byte_t low  = ReadByteFromMemory(REGISTER_PC++);
    byte_t high = ReadByteFromMemory(REGISTER_PC++);

    // store current PC for return
    Push(REGISTER_PC);

    // jump to subroutine
    REGISTER_PC = WORD(low, high);
}
