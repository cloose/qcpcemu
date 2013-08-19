#ifndef Z80_OPCODE_TABLE_H
#define Z80_OPCODE_TABLE_H

namespace Z80 {

enum OperandType
{
    OP_NONE,
    OP_BYTE,
    OP_OFFSET,
    OP_WORD
};


typedef void (Alu::*OpcodeFunc)();


struct OpcodeEntry
{
    OpcodeFunc func;

    OperandType operandType;
    const char* format;

    OpcodeTable* table;
};


struct OpcodeTable
{
    int opcodeOffset;
    OpcodeEntry entries[256];
};

} // namespace Z80

#endif // Z80_OPCODE_TABLE_H
