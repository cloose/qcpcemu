#ifndef Z80_ALU_H
#define Z80_ALU_H

#include "types.h"

class IInputOutput;
class IMemory;

namespace Z80 {

struct OpcodeTable;

/** The Z80 flags */
enum Flags
{
    F_C = 1,        /**< Carry */
    F_N = 2,        /**< Sub / Add */
    F_PV = 4,       /**< Parity / Overflow */
    F_3 = 8,        /**< Reserved */
    F_H = 16,       /**< Half carry */
    F_5 = 32,       /**< Reserved */
    F_Z = 64,       /**< Zero */
    F_S = 128       /**< Sign */
};

enum Condition
{
    C_,
    C_Z,
    C_NZ,
    C_C,
    C_NC,
    C_M,
    C_P,
    C_PE,
    C_PO
};


class Alu
{
public:
    Alu(IMemory *mem, IInputOutput *io);

    void reset();

    unsigned execute();
    unsigned executeTStates(unsigned minTstates);

    void requestInterrupt(byte_t value = 0);
    void requestNMI();

    unsigned interruptHandler();

    bool isInterruptDeferred() const;
    void setInterruptDeferred(bool deferred);

    #include "opcodes_decl.h"

private:
    void initOpcodeTables();

    void unhalt();

    // TODO: better name
    void step();

    byte_t ioRead(word_t address);
    void ioWrite(word_t address, byte_t value);

    int doComplement(byte_t v);
    byte_t doArithmetic(byte_t value, int withCarry, int isSub);
    word_t doAddWord(word_t a1, word_t a2, int withCarry, int isSub);
    void doAND(byte_t value);
    void doOR(byte_t value);
    void doXOR(byte_t value);
    void doBIT(int b, byte_t val);
    void doBIT_r(int b, byte_t val);
    void doBIT_indexed(int b, word_t address);
    byte_t doSetRes(int bit, int pos, byte_t val);
    byte_t doIncDec(byte_t val, int isDec);
    byte_t doRLC(int adjFlags, byte_t val);
    byte_t doRL(int adjFlags, byte_t val);
    byte_t doRRC(int adjFlags, byte_t val);
    byte_t doRR(int adjFlags, byte_t val);
    byte_t doSL(byte_t val, int isArith);
    byte_t doSR(byte_t val, int isArith);
    void doPush(ushort val);
    ushort doPop();
    byte_t doCP_HL();
    void doDAA();

    int condition(Condition cond);

    void setFlag(Flags flag);
    void resFlag(Flags flag);
    void valFlag(Flags flag, int val);
    int getFlag(Flags flag);
    void adjustFlags(byte_t val);
    void adjustFlagSZP(byte_t val);
    void adjustLogicFlag (int flagH);

    IMemory *memory;
    IInputOutput *ioHandler;
    unsigned tstates;
    unsigned interruptMode;
    bool halted;
    bool deferInterrupt;

    bool nmiRequested;
    bool interruptRequested;
    byte_t interruptVector;

    OpcodeTable *opcodes_main;
    OpcodeTable *opcodes_dd;
    OpcodeTable *opcodes_fd;
    OpcodeTable *opcodes_ed;
    OpcodeTable *opcodes_cb;
    OpcodeTable *opcodes_ddcb;
    OpcodeTable *opcodes_fdcb;
};

inline Flags operator|(Flags lhs, Flags rhs) {
    return (Flags)((int)lhs | (int)rhs);
}

} // namespace Z80

#endif // Z80_ALU_H
