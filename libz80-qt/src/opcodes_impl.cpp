#include "alu.h"
using namespace Z80;

#include "imemory.h"
#include "registers.h"


static int parityBit[256] = {
1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1 };

/* Flags for doIncDec() */
static const int ID_INC = 0;
static const int ID_DEC = 1;

/* Flags for enable / disable interrupts */
static const int IE_DI = 0;
static const int IE_EI = 1;

/* Flags for doSetRes() */
static const int SR_RES = 0;
static const int SR_SET = 1;

/* Flags for logical / arithmetic operations */
static const int IA_L = 0;
static const int IA_A = 1;

/* Flags for doArithmetic() - F1 = withCarry, F2 = isSub */
static const int F1_ADC = 1;
static const int F1_SBC = 1;
static const int F1_ADD = 0;
static const int F1_SUB = 0;

static const int F2_ADC = 0;
static const int F2_SBC = 1;
static const int F2_ADD = 0;
static const int F2_SUB = 1;

void Alu::ADC_A_off_HL()
{
	BR.A = doArithmetic(memory->read8(WR.HL), F1_ADC, F2_ADC);
}


void Alu::ADC_A_off_IX_d()
{
	tstates += 5;
	char displacement = memory->read8(Register::PC++);
	BR.A = doArithmetic(memory->read8(WR.IX + displacement), F1_ADC, F2_ADC);
	
}


void Alu::ADC_A_off_IY_d()
{
	tstates += 5;
	char displacement = memory->read8(Register::PC++);
	BR.A = doArithmetic(memory->read8(WR.IY + displacement), F1_ADC, F2_ADC);
	
}


void Alu::ADC_A_A()
{
	BR.A = doArithmetic(BR.A, F1_ADC, F2_ADC);
}


void Alu::ADC_A_B()
{
	BR.A = doArithmetic(BR.B, F1_ADC, F2_ADC);
}


void Alu::ADC_A_C()
{
	BR.A = doArithmetic(BR.C, F1_ADC, F2_ADC);
}


void Alu::ADC_A_D()
{
	BR.A = doArithmetic(BR.D, F1_ADC, F2_ADC);
}


void Alu::ADC_A_E()
{
	BR.A = doArithmetic(BR.E, F1_ADC, F2_ADC);
}


void Alu::ADC_A_H()
{
	BR.A = doArithmetic(BR.H, F1_ADC, F2_ADC);
}


void Alu::ADC_A_IXh()
{
	BR.A = doArithmetic(BR.IXh, F1_ADC, F2_ADC);
}


void Alu::ADC_A_IXl()
{
	BR.A = doArithmetic(BR.IXl, F1_ADC, F2_ADC);
}


void Alu::ADC_A_IYh()
{
	BR.A = doArithmetic(BR.IYh, F1_ADC, F2_ADC);
}


void Alu::ADC_A_IYl()
{
	BR.A = doArithmetic(BR.IYl, F1_ADC, F2_ADC);
}


void Alu::ADC_A_L()
{
	BR.A = doArithmetic(BR.L, F1_ADC, F2_ADC);
}


void Alu::ADC_A_n()
{
	BR.A = doArithmetic(memory->read8(Register::PC++), F1_ADC, F2_ADC);
}


void Alu::ADC_HL_BC()
{
	tstates += 7;
	WR.HL = doAddWord(WR.HL, WR.BC, F1_ADC, F2_ADC);
}


void Alu::ADC_HL_DE()
{
	tstates += 7;
	WR.HL = doAddWord(WR.HL, WR.DE, F1_ADC, F2_ADC);
}


void Alu::ADC_HL_HL()
{
	tstates += 7;
	WR.HL = doAddWord(WR.HL, WR.HL, F1_ADC, F2_ADC);
}


void Alu::ADC_HL_SP()
{
	tstates += 7;
	WR.HL = doAddWord(WR.HL, WR.SP, F1_ADC, F2_ADC);
}


void Alu::ADD_A_off_HL()
{
	BR.A = doArithmetic(memory->read8(WR.HL), F1_ADD, F2_ADD);
}


void Alu::ADD_A_off_IX_d()
{
	tstates += 5;
	char displacement = memory->read8(Register::PC++);
	BR.A = doArithmetic(memory->read8(WR.IX + displacement), F1_ADD, F2_ADD);
	
}


void Alu::ADD_A_off_IY_d()
{
	tstates += 5;
	char displacement = memory->read8(Register::PC++);
	BR.A = doArithmetic(memory->read8(WR.IY + displacement), F1_ADD, F2_ADD);
	
}


void Alu::ADD_A_A()
{
	BR.A = doArithmetic(BR.A, F1_ADD, F2_ADD);
}


void Alu::ADD_A_B()
{
	BR.A = doArithmetic(BR.B, F1_ADD, F2_ADD);
}


void Alu::ADD_A_C()
{
	BR.A = doArithmetic(BR.C, F1_ADD, F2_ADD);
}


void Alu::ADD_A_D()
{
	BR.A = doArithmetic(BR.D, F1_ADD, F2_ADD);
}


void Alu::ADD_A_E()
{
	BR.A = doArithmetic(BR.E, F1_ADD, F2_ADD);
}


void Alu::ADD_A_H()
{
	BR.A = doArithmetic(BR.H, F1_ADD, F2_ADD);
}


void Alu::ADD_A_IXh()
{
	BR.A = doArithmetic(BR.IXh, F1_ADD, F2_ADD);
}


void Alu::ADD_A_IXl()
{
	BR.A = doArithmetic(BR.IXl, F1_ADD, F2_ADD);
}


void Alu::ADD_A_IYh()
{
	BR.A = doArithmetic(BR.IYh, F1_ADD, F2_ADD);
}


void Alu::ADD_A_IYl()
{
	BR.A = doArithmetic(BR.IYl, F1_ADD, F2_ADD);
}


void Alu::ADD_A_L()
{
	BR.A = doArithmetic(BR.L, F1_ADD, F2_ADD);
}


void Alu::ADD_A_n()
{
	BR.A = doArithmetic(memory->read8(Register::PC++), F1_ADD, F2_ADD);
}


void Alu::ADD_HL_BC()
{
	tstates += 7;
	WR.HL = doAddWord(WR.HL, WR.BC, F1_ADD, F2_ADD);
}


void Alu::ADD_HL_DE()
{
	tstates += 7;
	WR.HL = doAddWord(WR.HL, WR.DE, F1_ADD, F2_ADD);
}


void Alu::ADD_HL_HL()
{
	tstates += 7;
	WR.HL = doAddWord(WR.HL, WR.HL, F1_ADD, F2_ADD);
}


void Alu::ADD_HL_SP()
{
	tstates += 7;
	WR.HL = doAddWord(WR.HL, WR.SP, F1_ADD, F2_ADD);
}


void Alu::ADD_IX_BC()
{
	tstates += 7;
	WR.IX = doAddWord(WR.IX, WR.BC, 0, 0);
}


void Alu::ADD_IX_DE()
{
	tstates += 7;
	WR.IX = doAddWord(WR.IX, WR.DE, 0, 0);
}


void Alu::ADD_IX_IX()
{
	tstates += 7;
	WR.IX = doAddWord(WR.IX, WR.IX, 0, 0);
}


void Alu::ADD_IX_SP()
{
	tstates += 7;
	WR.IX = doAddWord(WR.IX, WR.SP, 0, 0);
}


void Alu::ADD_IY_BC()
{
	tstates += 7;
	WR.IY = doAddWord(WR.IY, WR.BC, 0, 0);
}


void Alu::ADD_IY_DE()
{
	tstates += 7;
	WR.IY = doAddWord(WR.IY, WR.DE, 0, 0);
}


void Alu::ADD_IY_IY()
{
	tstates += 7;
	WR.IY = doAddWord(WR.IY, WR.IY, 0, 0);
}


void Alu::ADD_IY_SP()
{
	tstates += 7;
	WR.IY = doAddWord(WR.IY, WR.SP, 0, 0);
}


void Alu::AND_off_HL()
{
	doAND(memory->read8(WR.HL));
}


void Alu::AND_off_IX_d()
{
	tstates += 5;
	doAND(memory->read8(WR.IX + (char) memory->read8(Register::PC++)));
}


void Alu::AND_off_IY_d()
{
	tstates += 5;
	doAND(memory->read8(WR.IY + (char) memory->read8(Register::PC++)));
}


void Alu::AND_A()
{
	doAND(BR.A);
}


void Alu::AND_B()
{
	doAND(BR.B);
}


void Alu::AND_C()
{
	doAND(BR.C);
}


void Alu::AND_D()
{
	doAND(BR.D);
}


void Alu::AND_E()
{
	doAND(BR.E);
}


void Alu::AND_H()
{
	doAND(BR.H);
}


void Alu::AND_IXh()
{
	doAND(BR.IXh);
}


void Alu::AND_IXl()
{
	doAND(BR.IXl);
}


void Alu::AND_IYh()
{
	doAND(BR.IYh);
}


void Alu::AND_IYl()
{
	doAND(BR.IYl);
}


void Alu::AND_L()
{
	doAND(BR.L);
}


void Alu::AND_n()
{
	doAND(memory->read8(Register::PC++));
}


void Alu::BIT_0_off_HL()
{
	tstates += 1;
	doBIT_r(0, memory->read8(WR.HL));
}


void Alu::BIT_0_off_IX_d()
{
	tstates += 2;
	ushort address = WR.IX + (char) memory->read8(Register::PC++);
	doBIT_indexed(0, address);
}


void Alu::BIT_0_off_IY_d()
{
	tstates += 2;
	ushort address = WR.IY + (char) memory->read8(Register::PC++);
	doBIT_indexed(0, address);
}


void Alu::BIT_0_A()
{
	doBIT_r(0, BR.A);
}


void Alu::BIT_0_B()
{
	doBIT_r(0, BR.B);
}


void Alu::BIT_0_C()
{
	doBIT_r(0, BR.C);
}


void Alu::BIT_0_D()
{
	doBIT_r(0, BR.D);
}


void Alu::BIT_0_E()
{
	doBIT_r(0, BR.E);
}


void Alu::BIT_0_H()
{
	doBIT_r(0, BR.H);
}


void Alu::BIT_0_L()
{
	doBIT_r(0, BR.L);
}


void Alu::BIT_1_off_HL()
{
	tstates += 1;
	doBIT_r(1, memory->read8(WR.HL));
}


void Alu::BIT_1_off_IX_d()
{
	tstates += 2;
	ushort address = WR.IX + (char) memory->read8(Register::PC++);
	doBIT_indexed(1, address);
}


void Alu::BIT_1_off_IY_d()
{
	tstates += 2;
	ushort address = WR.IY + (char) memory->read8(Register::PC++);
	doBIT_indexed(1, address);
}


void Alu::BIT_1_A()
{
	doBIT_r(1, BR.A);
}


void Alu::BIT_1_B()
{
	doBIT_r(1, BR.B);
}


void Alu::BIT_1_C()
{
	doBIT_r(1, BR.C);
}


void Alu::BIT_1_D()
{
	doBIT_r(1, BR.D);
}


void Alu::BIT_1_E()
{
	doBIT_r(1, BR.E);
}


void Alu::BIT_1_H()
{
	doBIT_r(1, BR.H);
}


void Alu::BIT_1_L()
{
	doBIT_r(1, BR.L);
}


void Alu::BIT_2_off_HL()
{
	tstates += 1;
	doBIT_r(2, memory->read8(WR.HL));
}


void Alu::BIT_2_off_IX_d()
{
	tstates += 2;
	ushort address = WR.IX + (char) memory->read8(Register::PC++);
	doBIT_indexed(2, address);
}


void Alu::BIT_2_off_IY_d()
{
	tstates += 2;
	ushort address = WR.IY + (char) memory->read8(Register::PC++);
	doBIT_indexed(2, address);
}


void Alu::BIT_2_A()
{
	doBIT_r(2, BR.A);
}


void Alu::BIT_2_B()
{
	doBIT_r(2, BR.B);
}


void Alu::BIT_2_C()
{
	doBIT_r(2, BR.C);
}


void Alu::BIT_2_D()
{
	doBIT_r(2, BR.D);
}


void Alu::BIT_2_E()
{
	doBIT_r(2, BR.E);
}


void Alu::BIT_2_H()
{
	doBIT_r(2, BR.H);
}


void Alu::BIT_2_L()
{
	doBIT_r(2, BR.L);
}


void Alu::BIT_3_off_HL()
{
	tstates += 1;
	doBIT_r(3, memory->read8(WR.HL));
}


void Alu::BIT_3_off_IX_d()
{
	tstates += 2;
	ushort address = WR.IX + (char) memory->read8(Register::PC++);
	doBIT_indexed(3, address);
}


void Alu::BIT_3_off_IY_d()
{
	tstates += 2;
	ushort address = WR.IY + (char) memory->read8(Register::PC++);
	doBIT_indexed(3, address);
}


void Alu::BIT_3_A()
{
	doBIT_r(3, BR.A);
}


void Alu::BIT_3_B()
{
	doBIT_r(3, BR.B);
}


void Alu::BIT_3_C()
{
	doBIT_r(3, BR.C);
}


void Alu::BIT_3_D()
{
	doBIT_r(3, BR.D);
}


void Alu::BIT_3_E()
{
	doBIT_r(3, BR.E);
}


void Alu::BIT_3_H()
{
	doBIT_r(3, BR.H);
}


void Alu::BIT_3_L()
{
	doBIT_r(3, BR.L);
}


void Alu::BIT_4_off_HL()
{
	tstates += 1;
	doBIT_r(4, memory->read8(WR.HL));
}


void Alu::BIT_4_off_IX_d()
{
	tstates += 2;
	ushort address = WR.IX + (char) memory->read8(Register::PC++);
	doBIT_indexed(4, address);
}


void Alu::BIT_4_off_IY_d()
{
	tstates += 2;
	ushort address = WR.IY + (char) memory->read8(Register::PC++);
	doBIT_indexed(4, address);
}


void Alu::BIT_4_A()
{
	doBIT_r(4, BR.A);
}


void Alu::BIT_4_B()
{
	doBIT_r(4, BR.B);
}


void Alu::BIT_4_C()
{
	doBIT_r(4, BR.C);
}


void Alu::BIT_4_D()
{
	doBIT_r(4, BR.D);
}


void Alu::BIT_4_E()
{
	doBIT_r(4, BR.E);
}


void Alu::BIT_4_H()
{
	doBIT_r(4, BR.H);
}


void Alu::BIT_4_L()
{
	doBIT_r(4, BR.L);
}


void Alu::BIT_5_off_HL()
{
	tstates += 1;
	doBIT_r(5, memory->read8(WR.HL));
}


void Alu::BIT_5_off_IX_d()
{
	tstates += 2;
	ushort address = WR.IX + (char) memory->read8(Register::PC++);
	doBIT_indexed(5, address);
}


void Alu::BIT_5_off_IY_d()
{
	tstates += 2;
	ushort address = WR.IY + (char) memory->read8(Register::PC++);
	doBIT_indexed(5, address);
}


void Alu::BIT_5_A()
{
	doBIT_r(5, BR.A);
}


void Alu::BIT_5_B()
{
	doBIT_r(5, BR.B);
}


void Alu::BIT_5_C()
{
	doBIT_r(5, BR.C);
}


void Alu::BIT_5_D()
{
	doBIT_r(5, BR.D);
}


void Alu::BIT_5_E()
{
	doBIT_r(5, BR.E);
}


void Alu::BIT_5_H()
{
	doBIT_r(5, BR.H);
}


void Alu::BIT_5_L()
{
	doBIT_r(5, BR.L);
}


void Alu::BIT_6_off_HL()
{
	tstates += 1;
	doBIT_r(6, memory->read8(WR.HL));
}


void Alu::BIT_6_off_IX_d()
{
	tstates += 2;
	ushort address = WR.IX + (char) memory->read8(Register::PC++);
	doBIT_indexed(6, address);
}


void Alu::BIT_6_off_IY_d()
{
	tstates += 2;
	ushort address = WR.IY + (char) memory->read8(Register::PC++);
	doBIT_indexed(6, address);
}


void Alu::BIT_6_A()
{
	doBIT_r(6, BR.A);
}


void Alu::BIT_6_B()
{
	doBIT_r(6, BR.B);
}


void Alu::BIT_6_C()
{
	doBIT_r(6, BR.C);
}


void Alu::BIT_6_D()
{
	doBIT_r(6, BR.D);
}


void Alu::BIT_6_E()
{
	doBIT_r(6, BR.E);
}


void Alu::BIT_6_H()
{
	doBIT_r(6, BR.H);
}


void Alu::BIT_6_L()
{
	doBIT_r(6, BR.L);
}


void Alu::BIT_7_off_HL()
{
	tstates += 1;
	doBIT_r(7, memory->read8(WR.HL));
}


void Alu::BIT_7_off_IX_d()
{
	tstates += 2;
	ushort address = WR.IX + (char) memory->read8(Register::PC++);
	doBIT_indexed(7, address);
}


void Alu::BIT_7_off_IY_d()
{
	tstates += 2;
	ushort address = WR.IY + (char) memory->read8(Register::PC++);
	doBIT_indexed(7, address);
}


void Alu::BIT_7_A()
{
	doBIT_r(7, BR.A);
}


void Alu::BIT_7_B()
{
	doBIT_r(7, BR.B);
}


void Alu::BIT_7_C()
{
	doBIT_r(7, BR.C);
}


void Alu::BIT_7_D()
{
	doBIT_r(7, BR.D);
}


void Alu::BIT_7_E()
{
	doBIT_r(7, BR.E);
}


void Alu::BIT_7_H()
{
	doBIT_r(7, BR.H);
}


void Alu::BIT_7_L()
{
	doBIT_r(7, BR.L);
}


void Alu::CALL_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_))
	{
		tstates += 1;
		doPush(Register::PC);
		Register::PC = addr;
	}
	
}


void Alu::CALL_C_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_C))
	{
		tstates += 1;
		doPush(Register::PC);
		Register::PC = addr;
	}
	
}


void Alu::CALL_M_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_M))
	{
		tstates += 1;
		doPush(Register::PC);
		Register::PC = addr;
	}
	
}


void Alu::CALL_NC_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_NC))
	{
		tstates += 1;
		doPush(Register::PC);
		Register::PC = addr;
	}
	
}


void Alu::CALL_NZ_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_NZ))
	{
		tstates += 1;
		doPush(Register::PC);
		Register::PC = addr;
	}
	
}


void Alu::CALL_P_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_P))
	{
		tstates += 1;
		doPush(Register::PC);
		Register::PC = addr;
	}
	
}


void Alu::CALL_PE_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_PE))
	{
		tstates += 1;
		doPush(Register::PC);
		Register::PC = addr;
	}
	
}


void Alu::CALL_PO_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_PO))
	{
		tstates += 1;
		doPush(Register::PC);
		Register::PC = addr;
	}
	
}


void Alu::CALL_Z_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_Z))
	{
		tstates += 1;
		doPush(Register::PC);
		Register::PC = addr;
	}
	
}


void Alu::CCF()
{
	valFlag(F_C, (1 - (byte_t)getFlag(F_C) != 0));
	resFlag(F_N);
	adjustFlags(BR.A);
}


void Alu::CP_off_HL()
{
	doCP_HL();
}


void Alu::CP_off_IX_d()
{
	tstates += 5;
	char displacement = memory->read8(Register::PC++);
	byte_t val = memory->read8(WR.IX + displacement);
	doArithmetic(val, 0, 1);	
	adjustFlags(val);
}


void Alu::CP_off_IY_d()
{
	tstates += 5;
	char displacement = memory->read8(Register::PC++);
	byte_t val = memory->read8(WR.IY + displacement);
	doArithmetic(val, 0, 1);	
	adjustFlags(val);
}


void Alu::CP_A()
{
	doArithmetic(BR.A, 0, 1);	
	adjustFlags(BR.A);
}


void Alu::CP_B()
{
	doArithmetic(BR.B, 0, 1);	
	adjustFlags(BR.B);
}


void Alu::CP_C()
{
	doArithmetic(BR.C, 0, 1);	
	adjustFlags(BR.C);
}


void Alu::CP_D()
{
	doArithmetic(BR.D, 0, 1);	
	adjustFlags(BR.D);
}


void Alu::CP_E()
{
	doArithmetic(BR.E, 0, 1);	
	adjustFlags(BR.E);
}


void Alu::CP_H()
{
	doArithmetic(BR.H, 0, 1);	
	adjustFlags(BR.H);
}


void Alu::CP_IXh()
{
	doArithmetic(BR.IXh, 0, 1);	
	adjustFlags(BR.IXh);
}


void Alu::CP_IXl()
{
	doArithmetic(BR.IXl, 0, 1);	
	adjustFlags(BR.IXl);
}


void Alu::CP_IYh()
{
	doArithmetic(BR.IYh, 0, 1);	
	adjustFlags(BR.IYh);
}


void Alu::CP_IYl()
{
	doArithmetic(BR.IYl, 0, 1);	
	adjustFlags(BR.IYl);
}


void Alu::CP_L()
{
	doArithmetic(BR.L, 0, 1);	
	adjustFlags(BR.L);
}


void Alu::CP_n()
{
	byte_t val = memory->read8(Register::PC++);
	doArithmetic(val, 0, 1);	
	adjustFlags(val);
}


void Alu::CPD()
{
	tstates += 5;
	int carry = getFlag(F_C);
	byte_t value = doCP_HL();
	if(getFlag(F_H))
		value--;
	WR.HL--;
	WR.BC--;
	valFlag(F_PV, WR.BC != 0);
	if(carry)
		setFlag(F_C);
	else
		resFlag(F_C);
	valFlag(F_5, value & (1 << 1));
	valFlag(F_3, value & (1 << 3));
}


void Alu::CPDR()
{
	CPD();
	if (WR.BC != 0 && !getFlag(F_Z))
	{
		tstates += 5;
		Register::PC -= 2;
	}
}


void Alu::CPI()
{
	tstates += 5;
	int carry = getFlag(F_C);
	byte_t value = doCP_HL();
	if(getFlag(F_H))
		value--;
	WR.HL++;
	WR.BC--;
	valFlag(F_PV, WR.BC != 0);
	valFlag(F_C, carry);
	valFlag(F_5, value & (1 << 2));
	valFlag(F_3, value & (1 << 3));
}


void Alu::CPIR()
{
	CPI();
	if (WR.BC != 0 && !getFlag(F_Z))
	{
		tstates += 5;
		Register::PC -= 2;
	}
}


void Alu::CPL()
{
	BR.A = ~BR.A;
	setFlag(F_H | F_N);
	adjustFlags(BR.A);
	
}


void Alu::DAA()
{
	doDAA();
	
}


void Alu::DEC_off_HL()
{
	tstates += 1;
	byte_t value = memory->read8(WR.HL);
	memory->write8(WR.HL, doIncDec(value, ID_DEC));
}


void Alu::DEC_off_IX_d()
{
	tstates += 6;
	char off = memory->read8(Register::PC++);
	byte_t value = memory->read8(WR.IX + off);
	memory->write8(WR.IX + off, doIncDec(value, ID_DEC));
	
}


void Alu::DEC_off_IY_d()
{
	tstates += 6;
	char off = memory->read8(Register::PC++);
	byte_t value = memory->read8(WR.IY + off);
	memory->write8(WR.IY + off, doIncDec(value, ID_DEC));
	
}


void Alu::DEC_A()
{
	BR.A = doIncDec(BR.A, ID_DEC);
}


void Alu::DEC_B()
{
	BR.B = doIncDec(BR.B, ID_DEC);
}


void Alu::DEC_BC()
{
	tstates += 2;
	WR.BC--;
}


void Alu::DEC_C()
{
	BR.C = doIncDec(BR.C, ID_DEC);
}


void Alu::DEC_D()
{
	BR.D = doIncDec(BR.D, ID_DEC);
}


void Alu::DEC_DE()
{
	tstates += 2;
	WR.DE--;
}


void Alu::DEC_E()
{
	BR.E = doIncDec(BR.E, ID_DEC);
}


void Alu::DEC_H()
{
	BR.H = doIncDec(BR.H, ID_DEC);
}


void Alu::DEC_HL()
{
	tstates += 2;
	WR.HL--;
}


void Alu::DEC_IX()
{
	tstates += 2;
	WR.IX--;
}


void Alu::DEC_IXh()
{
	BR.IXh = doIncDec(BR.IXh, ID_DEC);
}


void Alu::DEC_IXl()
{
	BR.IXl = doIncDec(BR.IXl, ID_DEC);
}


void Alu::DEC_IY()
{
	tstates += 2;
	WR.IY--;
}


void Alu::DEC_IYh()
{
	BR.IYh = doIncDec(BR.IYh, ID_DEC);
}


void Alu::DEC_IYl()
{
	BR.IYl = doIncDec(BR.IYl, ID_DEC);
}


void Alu::DEC_L()
{
	BR.L = doIncDec(BR.L, ID_DEC);
}


void Alu::DEC_SP()
{
	tstates += 2;
	WR.SP--;
}


void Alu::DI()
{
	Register::IFF1 = Register::IFF2 = IE_DI;
    deferInterrupt = true;
}


void Alu::DJNZ_off_PC_e()
{
	tstates += 1;
	char off = memory->read8(Register::PC++);
	BR.B--;
	if (BR.B)
	{
		tstates += 5;
		Register::PC += off;
	}
}


void Alu::EI()
{
	Register::IFF1 = Register::IFF2 = IE_EI;
    deferInterrupt = true;
}


void Alu::EX_off_SP_HL()
{
	tstates += 3;
    ushort tmp = memory->read16(WR.SP);
    memory->write16(WR.SP, WR.HL);
	WR.HL = tmp;
}


void Alu::EX_off_SP_IX()
{
	tstates += 3;
    ushort tmp = memory->read16(WR.SP);
    memory->write16(WR.SP, WR.IX);
	WR.IX = tmp;
}


void Alu::EX_off_SP_IY()
{
	tstates += 3;
    ushort tmp = memory->read16(WR.SP);
    memory->write16(WR.SP, WR.IY);
	WR.IY = tmp;
}


void Alu::EX_AF_AF_()
{
	ushort tmp = WR.AF;
	WR.AF = Register::R2.wr.AF;
	Register::R2.wr.AF = tmp;
}


void Alu::EX_DE_HL()
{
	ushort tmp = WR.DE;
	WR.DE = WR.HL;
	WR.HL = tmp;
}


void Alu::EXX()
{
	ushort tmp;	
    tmp = WR.BC;
    WR.BC = Register::R2.wr.BC;
    Register::R2.wr.BC = tmp;
	
    tmp = WR.DE;
    WR.DE = Register::R2.wr.DE;
    Register::R2.wr.DE = tmp;
	
    tmp = WR.HL;
    WR.HL = Register::R2.wr.HL;
    Register::R2.wr.HL = tmp;
}


void Alu::HALT()
{
    halted = true;
	Register::PC--;
}


void Alu::IM_0()
{
	interruptMode = 0;
}


void Alu::IM_1()
{
	interruptMode = 1;
}


void Alu::IM_2()
{
	interruptMode = 2;
}


void Alu::IN_A_off_C()
{
	BR.A = ioRead(WR.BC);
	resFlag(F_H | F_N);
	adjustFlagSZP(BR.A);
	adjustFlags(BR.A);
}


void Alu::IN_A_off_n()
{
	byte_t port = memory->read8(Register::PC++);	
	BR.A = ioRead(BR.A << 8 | port);
}


void Alu::IN_B_off_C()
{
	BR.B = ioRead(WR.BC);
	resFlag(F_H | F_N);
	adjustFlagSZP(BR.B);
	adjustFlags(BR.B);
}


void Alu::IN_C_off_C()
{
	BR.C = ioRead(WR.BC);
	resFlag(F_H | F_N);
	adjustFlagSZP(BR.C);
	adjustFlags(BR.C);
}


void Alu::IN_D_off_C()
{
	BR.D = ioRead(WR.BC);
	resFlag(F_H | F_N);
	adjustFlagSZP(BR.D);
	adjustFlags(BR.D);
}


void Alu::IN_E_off_C()
{
	BR.E = ioRead(WR.BC);
	resFlag(F_H | F_N);
	adjustFlagSZP(BR.E);
	adjustFlags(BR.E);
}


void Alu::IN_F_off_C()
{
	BR.F = ioRead(WR.BC);
	resFlag(F_H | F_N);
	adjustFlagSZP(BR.F);
	adjustFlags(BR.F);
}


void Alu::IN_H_off_C()
{
	BR.H = ioRead(WR.BC);
	resFlag(F_H | F_N);
	adjustFlagSZP(BR.H);
	adjustFlags(BR.H);
}


void Alu::IN_L_off_C()
{
	BR.L = ioRead(WR.BC);
	resFlag(F_H | F_N);
	adjustFlagSZP(BR.L);
	adjustFlags(BR.L);
}


void Alu::INC_off_HL()
{
	tstates += 1;
	byte_t value = memory->read8(WR.HL);
	memory->write8(WR.HL, doIncDec(value, ID_INC));
}


void Alu::INC_off_IX_d()
{
	tstates += 6;
	char off = memory->read8(Register::PC++);
	byte_t value = memory->read8(WR.IX + off);
	memory->write8(WR.IX + off, doIncDec(value, ID_INC));
	
}


void Alu::INC_off_IY_d()
{
	tstates += 6;
	char off = memory->read8(Register::PC++);
	byte_t value = memory->read8(WR.IY + off);
	memory->write8(WR.IY + off, doIncDec(value, ID_INC));
	
}


void Alu::INC_A()
{
	BR.A = doIncDec(BR.A, ID_INC);
}


void Alu::INC_B()
{
	BR.B = doIncDec(BR.B, ID_INC);
}


void Alu::INC_BC()
{
	tstates += 2;
	WR.BC++;
}


void Alu::INC_C()
{
	BR.C = doIncDec(BR.C, ID_INC);
}


void Alu::INC_D()
{
	BR.D = doIncDec(BR.D, ID_INC);
}


void Alu::INC_DE()
{
	tstates += 2;
	WR.DE++;
}


void Alu::INC_E()
{
	BR.E = doIncDec(BR.E, ID_INC);
}


void Alu::INC_H()
{
	BR.H = doIncDec(BR.H, ID_INC);
}


void Alu::INC_HL()
{
	tstates += 2;
	WR.HL++;
}


void Alu::INC_IX()
{
	tstates += 2;
	WR.IX++;
}


void Alu::INC_IXh()
{
	BR.IXh = doIncDec(BR.IXh, ID_INC);
}


void Alu::INC_IXl()
{
	BR.IXl = doIncDec(BR.IXl, ID_INC);
}


void Alu::INC_IY()
{
	tstates += 2;
	WR.IY++;
}


void Alu::INC_IYh()
{
	BR.IYh = doIncDec(BR.IYh, ID_INC);
}


void Alu::INC_IYl()
{
	BR.IYl = doIncDec(BR.IYl, ID_INC);
}


void Alu::INC_L()
{
	BR.L = doIncDec(BR.L, ID_INC);
}


void Alu::INC_SP()
{
	tstates += 2;
	WR.SP++;
}


void Alu::IND()
{
	tstates += 1;
	byte_t val = ioRead(WR.BC);
	memory->write8(WR.HL, val);
	WR.HL--;
	BR.B = doIncDec(BR.B, ID_DEC);
	valFlag(F_N, (val & 0x80) != 0);
	int flagval = val + ((BR.C - 1) & 0xff);
	valFlag(F_H, flagval > 0xff);
	valFlag(F_C, flagval > 0xff);
	valFlag(F_PV, parityBit[(flagval & 7) ^ BR.B]);
}


void Alu::INDR()
{
	IND();
	if (BR.B != 0)
	{
		tstates += 5;
		Register::PC -= 2;
	}
}


void Alu::INI()
{
	tstates += 1;
	byte_t val = ioRead(WR.BC);
	memory->write8(WR.HL, val);
	WR.HL++;
	BR.B = doIncDec(BR.B, ID_DEC);
	valFlag(F_N, (val & 0x80) != 0);
	int flagval = val + ((BR.C + 1) & 0xff);
	valFlag(F_H, flagval > 0xff);
	valFlag(F_C, flagval > 0xff);
	valFlag(F_PV, parityBit[(flagval & 7) ^ BR.B]);
}


void Alu::INIR()
{
	INI();
	if (BR.B != 0)
	{
		tstates += 5;
		Register::PC -= 2;
	}
}


void Alu::JP_off_HL()
{
	Register::PC = WR.HL;
	
}


void Alu::JP_off_IX()
{
	Register::PC = WR.IX;
	
}


void Alu::JP_off_IY()
{
	Register::PC = WR.IY;
	
}


void Alu::JP_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_))
		Register::PC = addr;
	
}


void Alu::JP_C_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_C))
		Register::PC = addr;
	
}


void Alu::JP_M_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_M))
		Register::PC = addr;
	
}


void Alu::JP_NC_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_NC))
		Register::PC = addr;
	
}


void Alu::JP_NZ_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_NZ))
		Register::PC = addr;
	
}


void Alu::JP_P_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_P))
		Register::PC = addr;
	
}


void Alu::JP_PE_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_PE))
		Register::PC = addr;
	
}


void Alu::JP_PO_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_PO))
		Register::PC = addr;
	
}


void Alu::JP_Z_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_Z))
		Register::PC = addr;
	
}


void Alu::JR_off_PC_e()
{
	int off = doComplement(memory->read8(Register::PC++));
	if (condition(C_))
	{
		tstates += 5;
		Register::PC += off;
	}
}


void Alu::JR_C_off_PC_e()
{
	int off = doComplement(memory->read8(Register::PC++));
	if (condition(C_C))
	{
		tstates += 5;
		Register::PC += off;
	}
}


void Alu::JR_NC_off_PC_e()
{
	int off = doComplement(memory->read8(Register::PC++));
	if (condition(C_NC))
	{
		tstates += 5;
		Register::PC += off;
	}
}


void Alu::JR_NZ_off_PC_e()
{
	int off = doComplement(memory->read8(Register::PC++));
	if (condition(C_NZ))
	{
		tstates += 5;
		Register::PC += off;
	}
}


void Alu::JR_Z_off_PC_e()
{
	int off = doComplement(memory->read8(Register::PC++));
	if (condition(C_Z))
	{
		tstates += 5;
		Register::PC += off;
	}
}


void Alu::LD_off_BC_A()
{
	memory->write8(WR.BC, BR.A);
}


void Alu::LD_off_DE_A()
{
	memory->write8(WR.DE, BR.A);
}


void Alu::LD_off_HL_A()
{
	memory->write8(WR.HL, BR.A);
}


void Alu::LD_off_HL_B()
{
	memory->write8(WR.HL, BR.B);
}


void Alu::LD_off_HL_C()
{
	memory->write8(WR.HL, BR.C);
}


void Alu::LD_off_HL_D()
{
	memory->write8(WR.HL, BR.D);
}


void Alu::LD_off_HL_E()
{
	memory->write8(WR.HL, BR.E);
}


void Alu::LD_off_HL_H()
{
	memory->write8(WR.HL, BR.H);
}


void Alu::LD_off_HL_L()
{
	memory->write8(WR.HL, BR.L);
}


void Alu::LD_off_HL_n()
{
	memory->write8(WR.HL, memory->read8(Register::PC++));
	
}


void Alu::LD_off_IX_d_A()
{
	tstates += 5;
	memory->write8(WR.IX + (char) memory->read8(Register::PC++), BR.A);
	
}


void Alu::LD_off_IX_d_B()
{
	tstates += 5;
	memory->write8(WR.IX + (char) memory->read8(Register::PC++), BR.B);
	
}


void Alu::LD_off_IX_d_C()
{
	tstates += 5;
	memory->write8(WR.IX + (char) memory->read8(Register::PC++), BR.C);
	
}


void Alu::LD_off_IX_d_D()
{
	tstates += 5;
	memory->write8(WR.IX + (char) memory->read8(Register::PC++), BR.D);
	
}


void Alu::LD_off_IX_d_E()
{
	tstates += 5;
	memory->write8(WR.IX + (char) memory->read8(Register::PC++), BR.E);
	
}


void Alu::LD_off_IX_d_H()
{
	tstates += 5;
	memory->write8(WR.IX + (char) memory->read8(Register::PC++), BR.H);
	
}


void Alu::LD_off_IX_d_L()
{
	tstates += 5;
	memory->write8(WR.IX + (char) memory->read8(Register::PC++), BR.L);
	
}


void Alu::LD_off_IX_d_n()
{
	tstates += 2;
	char offset = memory->read8(Register::PC++);
	byte_t n = memory->read8(Register::PC++);
	memory->write8(WR.IX + offset, n);
	
}


void Alu::LD_off_IY_d_A()
{
	tstates += 5;
	memory->write8(WR.IY + (char) memory->read8(Register::PC++), BR.A);
	
}


void Alu::LD_off_IY_d_B()
{
	tstates += 5;
	memory->write8(WR.IY + (char) memory->read8(Register::PC++), BR.B);
	
}


void Alu::LD_off_IY_d_C()
{
	tstates += 5;
	memory->write8(WR.IY + (char) memory->read8(Register::PC++), BR.C);
	
}


void Alu::LD_off_IY_d_D()
{
	tstates += 5;
	memory->write8(WR.IY + (char) memory->read8(Register::PC++), BR.D);
	
}


void Alu::LD_off_IY_d_E()
{
	tstates += 5;
	memory->write8(WR.IY + (char) memory->read8(Register::PC++), BR.E);
	
}


void Alu::LD_off_IY_d_H()
{
	tstates += 5;
	memory->write8(WR.IY + (char) memory->read8(Register::PC++), BR.H);
	
}


void Alu::LD_off_IY_d_L()
{
	tstates += 5;
	memory->write8(WR.IY + (char) memory->read8(Register::PC++), BR.L);
	
}


void Alu::LD_off_IY_d_n()
{
	tstates += 2;
	char offset = memory->read8(Register::PC++);
	byte_t n = memory->read8(Register::PC++);
	memory->write8(WR.IY + offset, n);
	
}


void Alu::LD_off_nn_A()
{
	memory->write8(memory->read16(Register::PC), BR.A);
	Register::PC += 2;
	
}


void Alu::LD_off_nn_BC()
{
	memory->write16(memory->read16(Register::PC), WR.BC);
	Register::PC += 2;
	
}


void Alu::LD_off_nn_DE()
{
	memory->write16(memory->read16(Register::PC), WR.DE);
	Register::PC += 2;
	
}


void Alu::LD_off_nn_HL()
{
	memory->write16(memory->read16(Register::PC), WR.HL);
	Register::PC += 2;
	
}


void Alu::LD_off_nn_IX()
{
	memory->write16(memory->read16(Register::PC), WR.IX);
	Register::PC += 2;
	
}


void Alu::LD_off_nn_IY()
{
	memory->write16(memory->read16(Register::PC), WR.IY);
	Register::PC += 2;
	
}


void Alu::LD_off_nn_SP()
{
	memory->write16(memory->read16(Register::PC), WR.SP);
	Register::PC += 2;
	
}


void Alu::LD_A_off_BC()
{
	BR.A = memory->read8(WR.BC);
}


void Alu::LD_A_off_DE()
{
	BR.A = memory->read8(WR.DE);
}


void Alu::LD_A_off_HL()
{
	BR.A = memory->read8(WR.HL);
}


void Alu::LD_A_off_IX_d()
{
	tstates += 5;
	BR.A = memory->read8(WR.IX + (char) memory->read8(Register::PC++));
}


void Alu::LD_A_off_IY_d()
{
	tstates += 5;
	BR.A = memory->read8(WR.IY + (char) memory->read8(Register::PC++));
}


void Alu::LD_A_off_nn()
{
	BR.A = memory->read8(memory->read16(Register::PC));
	Register::PC += 2;
	
}


void Alu::LD_A_A()
{
	BR.A = BR.A;
}


void Alu::LD_A_B()
{
	BR.A = BR.B;
}


void Alu::LD_A_C()
{
	BR.A = BR.C;
}


void Alu::LD_A_D()
{
	BR.A = BR.D;
}


void Alu::LD_A_E()
{
	BR.A = BR.E;
}


void Alu::LD_A_H()
{
	BR.A = BR.H;
}


void Alu::LD_A_I()
{
	tstates += 1;
    BR.A = Register::I;
	adjustFlags(BR.A);
	resFlag(F_H | F_N);
	valFlag(F_PV, Register::IFF2);
	valFlag(F_S, (BR.A & 0x80) != 0);
	valFlag(F_Z, (BR.A == 0));
	
}


void Alu::LD_A_IXh()
{
	BR.A = BR.IXh;
}


void Alu::LD_A_IXl()
{
	BR.A = BR.IXl;
}


void Alu::LD_A_IYh()
{
	BR.A = BR.IYh;
}


void Alu::LD_A_IYl()
{
	BR.A = BR.IYl;
}


void Alu::LD_A_L()
{
	BR.A = BR.L;
}


void Alu::LD_A_n()
{
	BR.A = memory->read8(Register::PC++);
	
}


void Alu::LD_A_R()
{
	tstates += 1;
    BR.A = Register::R;
	adjustFlags(BR.A);
	resFlag(F_H | F_N);
	valFlag(F_PV, Register::IFF2);
	valFlag(F_S, (BR.A & 0x80) != 0);
	valFlag(F_Z, (BR.A == 0));
	
}


void Alu::LD_A_RES_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 0, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_RES_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 0, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_RES_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_RES_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_RES_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 2, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_RES_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 2, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_RES_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 3, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_RES_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 3, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_RES_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 4, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_RES_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 4, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_RES_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 5, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_RES_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 5, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_RES_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 6, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_RES_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 6, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_RES_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 7, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_RES_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_RES, 7, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_RL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doRL(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);
}


void Alu::LD_A_RL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doRL(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);
}


void Alu::LD_A_RLC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doRLC(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);
}


void Alu::LD_A_RLC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doRLC(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);
}


void Alu::LD_A_RR_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doRR(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);
}


void Alu::LD_A_RR_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doRR(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);
}


void Alu::LD_A_RRC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doRRC(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);
}


void Alu::LD_A_RRC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doRRC(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);
}


void Alu::LD_A_SET_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 0, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_SET_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 0, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_SET_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_SET_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_SET_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 2, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_SET_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 2, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_SET_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 3, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_SET_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 3, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_SET_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 4, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_SET_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 4, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_SET_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 5, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_SET_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 5, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_SET_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 6, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_SET_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 6, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_SET_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 7, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.A);	
}


void Alu::LD_A_SET_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSetRes(SR_SET, 7, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.A);	
}


void Alu::LD_A_SLA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSL(memory->read8(WR.IX + off), 1);
	memory->write8(WR.IX + off, BR.A);	
	
}


void Alu::LD_A_SLA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSL(memory->read8(WR.IY + off), 1);
	memory->write8(WR.IY + off, BR.A);	
	
}


void Alu::LD_A_SLL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSL(memory->read8(WR.IX + off), 0);
	memory->write8(WR.IX + off, BR.A);	
	  
}


void Alu::LD_A_SLL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSL(memory->read8(WR.IY + off), 0);
	memory->write8(WR.IY + off, BR.A);	
	  
}


void Alu::LD_A_SRA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSR(memory->read8(WR.IX + off), 1);
	memory->write8(WR.IX + off, BR.A);	
	
}


void Alu::LD_A_SRA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSR(memory->read8(WR.IY + off), 1);
	memory->write8(WR.IY + off, BR.A);	
	
}


void Alu::LD_A_SRL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSR(memory->read8(WR.IX + off), 0);
	memory->write8(WR.IX + off, BR.A);	
	  
}


void Alu::LD_A_SRL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.A = doSR(memory->read8(WR.IY + off), 0);
	memory->write8(WR.IY + off, BR.A);	
	  
}


void Alu::LD_B_off_HL()
{
	BR.B = memory->read8(WR.HL);
}


void Alu::LD_B_off_IX_d()
{
	tstates += 5;
	BR.B = memory->read8(WR.IX + (char) memory->read8(Register::PC++));
}


void Alu::LD_B_off_IY_d()
{
	tstates += 5;
	BR.B = memory->read8(WR.IY + (char) memory->read8(Register::PC++));
}


void Alu::LD_B_A()
{
	BR.B = BR.A;
}


void Alu::LD_B_B()
{
	BR.B = BR.B;
}


void Alu::LD_B_C()
{
	BR.B = BR.C;
}


void Alu::LD_B_D()
{
	BR.B = BR.D;
}


void Alu::LD_B_E()
{
	BR.B = BR.E;
}


void Alu::LD_B_H()
{
	BR.B = BR.H;
}


void Alu::LD_B_IXh()
{
	BR.B = BR.IXh;
}


void Alu::LD_B_IXl()
{
	BR.B = BR.IXl;
}


void Alu::LD_B_IYh()
{
	BR.B = BR.IYh;
}


void Alu::LD_B_IYl()
{
	BR.B = BR.IYl;
}


void Alu::LD_B_L()
{
	BR.B = BR.L;
}


void Alu::LD_B_n()
{
	BR.B = memory->read8(Register::PC++);
	
}


void Alu::LD_B_RES_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 0, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_RES_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 0, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_RES_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_RES_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_RES_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 2, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_RES_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 2, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_RES_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 3, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_RES_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 3, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_RES_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 4, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_RES_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 4, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_RES_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 5, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_RES_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 5, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_RES_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 6, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_RES_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 6, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_RES_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 7, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_RES_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_RES, 7, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_RL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doRL(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);
}


void Alu::LD_B_RL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doRL(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);
}


void Alu::LD_B_RLC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doRLC(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);
}


void Alu::LD_B_RLC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doRLC(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);
}


void Alu::LD_B_RR_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doRR(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);
}


void Alu::LD_B_RR_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doRR(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);
}


void Alu::LD_B_RRC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doRRC(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);
}


void Alu::LD_B_RRC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doRRC(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);
}


void Alu::LD_B_SET_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 0, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_SET_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 0, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_SET_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_SET_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_SET_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 2, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_SET_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 2, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_SET_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 3, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_SET_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 3, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_SET_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 4, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_SET_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 4, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_SET_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 5, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_SET_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 5, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_SET_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 6, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_SET_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 6, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_SET_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 7, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.B);	
}


void Alu::LD_B_SET_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSetRes(SR_SET, 7, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.B);	
}


void Alu::LD_B_SLA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSL(memory->read8(WR.IX + off), 1);
	memory->write8(WR.IX + off, BR.B);	
	
}


void Alu::LD_B_SLA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSL(memory->read8(WR.IY + off), 1);
	memory->write8(WR.IY + off, BR.B);	
	
}


void Alu::LD_B_SLL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSL(memory->read8(WR.IX + off), 0);
	memory->write8(WR.IX + off, BR.B);	
	  
}


void Alu::LD_B_SLL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSL(memory->read8(WR.IY + off), 0);
	memory->write8(WR.IY + off, BR.B);	
	  
}


void Alu::LD_B_SRA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSR(memory->read8(WR.IX + off), 1);
	memory->write8(WR.IX + off, BR.B);	
	
}


void Alu::LD_B_SRA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSR(memory->read8(WR.IY + off), 1);
	memory->write8(WR.IY + off, BR.B);	
	
}


void Alu::LD_B_SRL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSR(memory->read8(WR.IX + off), 0);
	memory->write8(WR.IX + off, BR.B);	
	  
}


void Alu::LD_B_SRL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.B = doSR(memory->read8(WR.IY + off), 0);
	memory->write8(WR.IY + off, BR.B);	
	  
}


void Alu::LD_BC_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	WR.BC = memory->read16(addr);	
}


void Alu::LD_BC_nn()
{
	WR.BC = memory->read16(Register::PC);
	Register::PC += 2;
	
}


void Alu::LD_C_off_HL()
{
	BR.C = memory->read8(WR.HL);
}


void Alu::LD_C_off_IX_d()
{
	tstates += 5;
	BR.C = memory->read8(WR.IX + (char) memory->read8(Register::PC++));
}


void Alu::LD_C_off_IY_d()
{
	tstates += 5;
	BR.C = memory->read8(WR.IY + (char) memory->read8(Register::PC++));
}


void Alu::LD_C_A()
{
	BR.C = BR.A;
}


void Alu::LD_C_B()
{
	BR.C = BR.B;
}


void Alu::LD_C_C()
{
	BR.C = BR.C;
}


void Alu::LD_C_D()
{
	BR.C = BR.D;
}


void Alu::LD_C_E()
{
	BR.C = BR.E;
}


void Alu::LD_C_H()
{
	BR.C = BR.H;
}


void Alu::LD_C_IXh()
{
	BR.C = BR.IXh;
}


void Alu::LD_C_IXl()
{
	BR.C = BR.IXl;
}


void Alu::LD_C_IYh()
{
	BR.C = BR.IYh;
}


void Alu::LD_C_IYl()
{
	BR.C = BR.IYl;
}


void Alu::LD_C_L()
{
	BR.C = BR.L;
}


void Alu::LD_C_n()
{
	BR.C = memory->read8(Register::PC++);
	
}


void Alu::LD_C_RES_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 0, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_RES_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 0, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_RES_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_RES_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_RES_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 2, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_RES_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 2, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_RES_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 3, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_RES_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 3, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_RES_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 4, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_RES_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 4, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_RES_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 5, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_RES_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 5, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_RES_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 6, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_RES_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 6, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_RES_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 7, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_RES_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_RES, 7, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_RL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doRL(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);
}


void Alu::LD_C_RL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doRL(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);
}


void Alu::LD_C_RLC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doRLC(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);
}


void Alu::LD_C_RLC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doRLC(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);
}


void Alu::LD_C_RR_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doRR(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);
}


void Alu::LD_C_RR_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doRR(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);
}


void Alu::LD_C_RRC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doRRC(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);
}


void Alu::LD_C_RRC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doRRC(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);
}


void Alu::LD_C_SET_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 0, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_SET_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 0, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_SET_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_SET_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_SET_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 2, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_SET_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 2, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_SET_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 3, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_SET_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 3, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_SET_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 4, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_SET_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 4, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_SET_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 5, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_SET_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 5, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_SET_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 6, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_SET_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 6, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_SET_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 7, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.C);	
}


void Alu::LD_C_SET_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSetRes(SR_SET, 7, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.C);	
}


void Alu::LD_C_SLA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSL(memory->read8(WR.IX + off), 1);
	memory->write8(WR.IX + off, BR.C);	
	
}


void Alu::LD_C_SLA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSL(memory->read8(WR.IY + off), 1);
	memory->write8(WR.IY + off, BR.C);	
	
}


void Alu::LD_C_SLL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSL(memory->read8(WR.IX + off), 0);
	memory->write8(WR.IX + off, BR.C);	
	  
}


void Alu::LD_C_SLL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSL(memory->read8(WR.IY + off), 0);
	memory->write8(WR.IY + off, BR.C);	
	  
}


void Alu::LD_C_SRA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSR(memory->read8(WR.IX + off), 1);
	memory->write8(WR.IX + off, BR.C);	
	
}


void Alu::LD_C_SRA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSR(memory->read8(WR.IY + off), 1);
	memory->write8(WR.IY + off, BR.C);	
	
}


void Alu::LD_C_SRL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSR(memory->read8(WR.IX + off), 0);
	memory->write8(WR.IX + off, BR.C);	
	  
}


void Alu::LD_C_SRL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.C = doSR(memory->read8(WR.IY + off), 0);
	memory->write8(WR.IY + off, BR.C);	
	  
}


void Alu::LD_D_off_HL()
{
	BR.D = memory->read8(WR.HL);
}


void Alu::LD_D_off_IX_d()
{
	tstates += 5;
	BR.D = memory->read8(WR.IX + (char) memory->read8(Register::PC++));
}


void Alu::LD_D_off_IY_d()
{
	tstates += 5;
	BR.D = memory->read8(WR.IY + (char) memory->read8(Register::PC++));
}


void Alu::LD_D_A()
{
	BR.D = BR.A;
}


void Alu::LD_D_B()
{
	BR.D = BR.B;
}


void Alu::LD_D_C()
{
	BR.D = BR.C;
}


void Alu::LD_D_D()
{
	BR.D = BR.D;
}


void Alu::LD_D_E()
{
	BR.D = BR.E;
}


void Alu::LD_D_H()
{
	BR.D = BR.H;
}


void Alu::LD_D_IXh()
{
	BR.D = BR.IXh;
}


void Alu::LD_D_IXl()
{
	BR.D = BR.IXl;
}


void Alu::LD_D_IYh()
{
	BR.D = BR.IYh;
}


void Alu::LD_D_IYl()
{
	BR.D = BR.IYl;
}


void Alu::LD_D_L()
{
	BR.D = BR.L;
}


void Alu::LD_D_n()
{
	BR.D = memory->read8(Register::PC++);
	
}


void Alu::LD_D_RES_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 0, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_RES_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 0, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_RES_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_RES_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_RES_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 2, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_RES_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 2, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_RES_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 3, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_RES_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 3, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_RES_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 4, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_RES_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 4, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_RES_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 5, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_RES_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 5, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_RES_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 6, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_RES_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 6, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_RES_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 7, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_RES_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_RES, 7, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_RL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doRL(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);
}


void Alu::LD_D_RL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doRL(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);
}


void Alu::LD_D_RLC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doRLC(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);
}


void Alu::LD_D_RLC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doRLC(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);
}


void Alu::LD_D_RR_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doRR(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);
}


void Alu::LD_D_RR_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doRR(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);
}


void Alu::LD_D_RRC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doRRC(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);
}


void Alu::LD_D_RRC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doRRC(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);
}


void Alu::LD_D_SET_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 0, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_SET_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 0, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_SET_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_SET_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_SET_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 2, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_SET_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 2, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_SET_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 3, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_SET_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 3, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_SET_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 4, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_SET_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 4, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_SET_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 5, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_SET_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 5, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_SET_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 6, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_SET_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 6, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_SET_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 7, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.D);	
}


void Alu::LD_D_SET_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSetRes(SR_SET, 7, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.D);	
}


void Alu::LD_D_SLA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSL(memory->read8(WR.IX + off), 1);
	memory->write8(WR.IX + off, BR.D);	
	
}


void Alu::LD_D_SLA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSL(memory->read8(WR.IY + off), 1);
	memory->write8(WR.IY + off, BR.D);	
	
}


void Alu::LD_D_SLL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSL(memory->read8(WR.IX + off), 0);
	memory->write8(WR.IX + off, BR.D);	
	  
}


void Alu::LD_D_SLL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSL(memory->read8(WR.IY + off), 0);
	memory->write8(WR.IY + off, BR.D);	
	  
}


void Alu::LD_D_SRA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSR(memory->read8(WR.IX + off), 1);
	memory->write8(WR.IX + off, BR.D);	
	
}


void Alu::LD_D_SRA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSR(memory->read8(WR.IY + off), 1);
	memory->write8(WR.IY + off, BR.D);	
	
}


void Alu::LD_D_SRL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSR(memory->read8(WR.IX + off), 0);
	memory->write8(WR.IX + off, BR.D);	
	  
}


void Alu::LD_D_SRL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.D = doSR(memory->read8(WR.IY + off), 0);
	memory->write8(WR.IY + off, BR.D);	
	  
}


void Alu::LD_DE_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	WR.DE = memory->read16(addr);	
}


void Alu::LD_DE_nn()
{
	WR.DE = memory->read16(Register::PC);
	Register::PC += 2;
	
}


void Alu::LD_E_off_HL()
{
	BR.E = memory->read8(WR.HL);
}


void Alu::LD_E_off_IX_d()
{
	tstates += 5;
	BR.E = memory->read8(WR.IX + (char) memory->read8(Register::PC++));
}


void Alu::LD_E_off_IY_d()
{
	tstates += 5;
	BR.E = memory->read8(WR.IY + (char) memory->read8(Register::PC++));
}


void Alu::LD_E_A()
{
	BR.E = BR.A;
}


void Alu::LD_E_B()
{
	BR.E = BR.B;
}


void Alu::LD_E_C()
{
	BR.E = BR.C;
}


void Alu::LD_E_D()
{
	BR.E = BR.D;
}


void Alu::LD_E_E()
{
	BR.E = BR.E;
}


void Alu::LD_E_H()
{
	BR.E = BR.H;
}


void Alu::LD_E_IXh()
{
	BR.E = BR.IXh;
}


void Alu::LD_E_IXl()
{
	BR.E = BR.IXl;
}


void Alu::LD_E_IYh()
{
	BR.E = BR.IYh;
}


void Alu::LD_E_IYl()
{
	BR.E = BR.IYl;
}


void Alu::LD_E_L()
{
	BR.E = BR.L;
}


void Alu::LD_E_n()
{
	BR.E = memory->read8(Register::PC++);
	
}


void Alu::LD_E_RES_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 0, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_RES_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 0, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_RES_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_RES_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_RES_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 2, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_RES_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 2, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_RES_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 3, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_RES_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 3, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_RES_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 4, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_RES_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 4, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_RES_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 5, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_RES_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 5, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_RES_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 6, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_RES_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 6, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_RES_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 7, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_RES_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_RES, 7, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_RL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doRL(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);
}


void Alu::LD_E_RL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doRL(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);
}


void Alu::LD_E_RLC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doRLC(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);
}


void Alu::LD_E_RLC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doRLC(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);
}


void Alu::LD_E_RR_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doRR(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);
}


void Alu::LD_E_RR_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doRR(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);
}


void Alu::LD_E_RRC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doRRC(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);
}


void Alu::LD_E_RRC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doRRC(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);
}


void Alu::LD_E_SET_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 0, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_SET_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 0, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_SET_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_SET_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_SET_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 2, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_SET_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 2, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_SET_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 3, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_SET_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 3, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_SET_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 4, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_SET_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 4, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_SET_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 5, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_SET_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 5, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_SET_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 6, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_SET_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 6, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_SET_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 7, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.E);	
}


void Alu::LD_E_SET_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSetRes(SR_SET, 7, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.E);	
}


void Alu::LD_E_SLA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSL(memory->read8(WR.IX + off), 1);
	memory->write8(WR.IX + off, BR.E);	
	
}


void Alu::LD_E_SLA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSL(memory->read8(WR.IY + off), 1);
	memory->write8(WR.IY + off, BR.E);	
	
}


void Alu::LD_E_SLL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSL(memory->read8(WR.IX + off), 0);
	memory->write8(WR.IX + off, BR.E);	
	  
}


void Alu::LD_E_SLL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSL(memory->read8(WR.IY + off), 0);
	memory->write8(WR.IY + off, BR.E);	
	  
}


void Alu::LD_E_SRA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSR(memory->read8(WR.IX + off), 1);
	memory->write8(WR.IX + off, BR.E);	
	
}


void Alu::LD_E_SRA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSR(memory->read8(WR.IY + off), 1);
	memory->write8(WR.IY + off, BR.E);	
	
}


void Alu::LD_E_SRL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSR(memory->read8(WR.IX + off), 0);
	memory->write8(WR.IX + off, BR.E);	
	  
}


void Alu::LD_E_SRL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.E = doSR(memory->read8(WR.IY + off), 0);
	memory->write8(WR.IY + off, BR.E);	
	  
}


void Alu::LD_H_off_HL()
{
	BR.H = memory->read8(WR.HL);
}


void Alu::LD_H_off_IX_d()
{
	tstates += 5;
	BR.H = memory->read8(WR.IX + (char) memory->read8(Register::PC++));
}


void Alu::LD_H_off_IY_d()
{
	tstates += 5;
	BR.H = memory->read8(WR.IY + (char) memory->read8(Register::PC++));
}


void Alu::LD_H_A()
{
	BR.H = BR.A;
}


void Alu::LD_H_B()
{
	BR.H = BR.B;
}


void Alu::LD_H_C()
{
	BR.H = BR.C;
}


void Alu::LD_H_D()
{
	BR.H = BR.D;
}


void Alu::LD_H_E()
{
	BR.H = BR.E;
}


void Alu::LD_H_H()
{
	BR.H = BR.H;
}


void Alu::LD_H_L()
{
	BR.H = BR.L;
}


void Alu::LD_H_n()
{
	BR.H = memory->read8(Register::PC++);
	
}


void Alu::LD_H_RES_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 0, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_RES_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 0, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_RES_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_RES_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_RES_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 2, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_RES_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 2, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_RES_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 3, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_RES_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 3, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_RES_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 4, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_RES_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 4, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_RES_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 5, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_RES_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 5, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_RES_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 6, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_RES_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 6, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_RES_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 7, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_RES_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_RES, 7, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_RL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doRL(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);
}


void Alu::LD_H_RL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doRL(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);
}


void Alu::LD_H_RLC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doRLC(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);
}


void Alu::LD_H_RLC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doRLC(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);
}


void Alu::LD_H_RR_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doRR(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);
}


void Alu::LD_H_RR_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doRR(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);
}


void Alu::LD_H_RRC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doRRC(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);
}


void Alu::LD_H_RRC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doRRC(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);
}


void Alu::LD_H_SET_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 0, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_SET_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 0, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_SET_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_SET_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_SET_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 2, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_SET_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 2, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_SET_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 3, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_SET_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 3, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_SET_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 4, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_SET_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 4, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_SET_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 5, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_SET_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 5, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_SET_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 6, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_SET_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 6, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_SET_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 7, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.H);	
}


void Alu::LD_H_SET_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSetRes(SR_SET, 7, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.H);	
}


void Alu::LD_H_SLA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSL(memory->read8(WR.IX + off), 1);
	memory->write8(WR.IX + off, BR.H);	
	
}


void Alu::LD_H_SLA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSL(memory->read8(WR.IY + off), 1);
	memory->write8(WR.IY + off, BR.H);	
	
}


void Alu::LD_H_SLL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSL(memory->read8(WR.IX + off), 0);
	memory->write8(WR.IX + off, BR.H);	
	  
}


void Alu::LD_H_SLL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSL(memory->read8(WR.IY + off), 0);
	memory->write8(WR.IY + off, BR.H);	
	  
}


void Alu::LD_H_SRA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSR(memory->read8(WR.IX + off), 1);
	memory->write8(WR.IX + off, BR.H);	
	
}


void Alu::LD_H_SRA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSR(memory->read8(WR.IY + off), 1);
	memory->write8(WR.IY + off, BR.H);	
	
}


void Alu::LD_H_SRL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSR(memory->read8(WR.IX + off), 0);
	memory->write8(WR.IX + off, BR.H);	
	  
}


void Alu::LD_H_SRL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.H = doSR(memory->read8(WR.IY + off), 0);
	memory->write8(WR.IY + off, BR.H);	
	  
}


void Alu::LD_HL_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	WR.HL = memory->read16(addr);	
}


void Alu::LD_HL_nn()
{
	WR.HL = memory->read16(Register::PC);
	Register::PC += 2;
	
}


void Alu::LD_I_A()
{
	tstates += 1;
    Register::I = BR.A;
}


void Alu::LD_IX_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	WR.IX = memory->read16(addr);	
}


void Alu::LD_IX_nn()
{
	WR.IX = memory->read16(Register::PC);
	Register::PC += 2;
	
}


void Alu::LD_IXh_A()
{
	BR.IXh = BR.A;
}


void Alu::LD_IXh_B()
{
	BR.IXh = BR.B;
}


void Alu::LD_IXh_C()
{
	BR.IXh = BR.C;
}


void Alu::LD_IXh_D()
{
	BR.IXh = BR.D;
}


void Alu::LD_IXh_E()
{
	BR.IXh = BR.E;
}


void Alu::LD_IXh_IXh()
{
	BR.IXh = BR.IXh;
}


void Alu::LD_IXh_IXl()
{
	BR.IXh = BR.IXl;
}


void Alu::LD_IXh_n()
{
	BR.IXh = memory->read8(Register::PC++);
	
}


void Alu::LD_IXl_A()
{
	BR.IXl = BR.A;
}


void Alu::LD_IXl_B()
{
	BR.IXl = BR.B;
}


void Alu::LD_IXl_C()
{
	BR.IXl = BR.C;
}


void Alu::LD_IXl_D()
{
	BR.IXl = BR.D;
}


void Alu::LD_IXl_E()
{
	BR.IXl = BR.E;
}


void Alu::LD_IXl_IXh()
{
	BR.IXl = BR.IXh;
}


void Alu::LD_IXl_IXl()
{
	BR.IXl = BR.IXl;
}


void Alu::LD_IXl_n()
{
	BR.IXl = memory->read8(Register::PC++);
	
}


void Alu::LD_IY_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	WR.IY = memory->read16(addr);	
}


void Alu::LD_IY_nn()
{
	WR.IY = memory->read16(Register::PC);
	Register::PC += 2;
	
}


void Alu::LD_IYh_A()
{
	BR.IYh = BR.A;
}


void Alu::LD_IYh_B()
{
	BR.IYh = BR.B;
}


void Alu::LD_IYh_C()
{
	BR.IYh = BR.C;
}


void Alu::LD_IYh_D()
{
	BR.IYh = BR.D;
}


void Alu::LD_IYh_E()
{
	BR.IYh = BR.E;
}


void Alu::LD_IYh_IYh()
{
	BR.IYh = BR.IYh;
}


void Alu::LD_IYh_IYl()
{
	BR.IYh = BR.IYl;
}


void Alu::LD_IYh_n()
{
	BR.IYh = memory->read8(Register::PC++);
	
}


void Alu::LD_IYl_A()
{
	BR.IYl = BR.A;
}


void Alu::LD_IYl_B()
{
	BR.IYl = BR.B;
}


void Alu::LD_IYl_C()
{
	BR.IYl = BR.C;
}


void Alu::LD_IYl_D()
{
	BR.IYl = BR.D;
}


void Alu::LD_IYl_E()
{
	BR.IYl = BR.E;
}


void Alu::LD_IYl_IYh()
{
	BR.IYl = BR.IYh;
}


void Alu::LD_IYl_IYl()
{
	BR.IYl = BR.IYl;
}


void Alu::LD_IYl_n()
{
	BR.IYl = memory->read8(Register::PC++);
	
}


void Alu::LD_L_off_HL()
{
	BR.L = memory->read8(WR.HL);
}


void Alu::LD_L_off_IX_d()
{
	tstates += 5;
	BR.L = memory->read8(WR.IX + (char) memory->read8(Register::PC++));
}


void Alu::LD_L_off_IY_d()
{
	tstates += 5;
	BR.L = memory->read8(WR.IY + (char) memory->read8(Register::PC++));
}


void Alu::LD_L_A()
{
	BR.L = BR.A;
}


void Alu::LD_L_B()
{
	BR.L = BR.B;
}


void Alu::LD_L_C()
{
	BR.L = BR.C;
}


void Alu::LD_L_D()
{
	BR.L = BR.D;
}


void Alu::LD_L_E()
{
	BR.L = BR.E;
}


void Alu::LD_L_H()
{
	BR.L = BR.H;
}


void Alu::LD_L_L()
{
	BR.L = BR.L;
}


void Alu::LD_L_n()
{
	BR.L = memory->read8(Register::PC++);
	
}


void Alu::LD_L_RES_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 0, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_RES_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 0, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_RES_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_RES_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_RES_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 2, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_RES_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 2, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_RES_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 3, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_RES_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 3, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_RES_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 4, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_RES_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 4, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_RES_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 5, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_RES_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 5, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_RES_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 6, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_RES_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 6, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_RES_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 7, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_RES_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_RES, 7, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_RL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doRL(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);
}


void Alu::LD_L_RL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doRL(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);
}


void Alu::LD_L_RLC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doRLC(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);
}


void Alu::LD_L_RLC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doRLC(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);
}


void Alu::LD_L_RR_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doRR(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);
}


void Alu::LD_L_RR_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doRR(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);
}


void Alu::LD_L_RRC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doRRC(1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);
}


void Alu::LD_L_RRC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doRRC(1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);
}


void Alu::LD_L_SET_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 0, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_SET_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 0, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_SET_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 1, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_SET_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 1, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_SET_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 2, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_SET_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 2, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_SET_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 3, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_SET_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 3, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_SET_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 4, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_SET_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 4, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_SET_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 5, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_SET_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 5, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_SET_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 6, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_SET_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 6, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_SET_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 7, memory->read8(WR.IX + off));
	memory->write8(WR.IX + off, BR.L);	
}


void Alu::LD_L_SET_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSetRes(SR_SET, 7, memory->read8(WR.IY + off));
	memory->write8(WR.IY + off, BR.L);	
}


void Alu::LD_L_SLA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSL(memory->read8(WR.IX + off), 1);
	memory->write8(WR.IX + off, BR.L);	
	
}


void Alu::LD_L_SLA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSL(memory->read8(WR.IY + off), 1);
	memory->write8(WR.IY + off, BR.L);	
	
}


void Alu::LD_L_SLL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSL(memory->read8(WR.IX + off), 0);
	memory->write8(WR.IX + off, BR.L);	
	  
}


void Alu::LD_L_SLL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSL(memory->read8(WR.IY + off), 0);
	memory->write8(WR.IY + off, BR.L);	
	  
}


void Alu::LD_L_SRA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSR(memory->read8(WR.IX + off), 1);
	memory->write8(WR.IX + off, BR.L);	
	
}


void Alu::LD_L_SRA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSR(memory->read8(WR.IY + off), 1);
	memory->write8(WR.IY + off, BR.L);	
	
}


void Alu::LD_L_SRL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSR(memory->read8(WR.IX + off), 0);
	memory->write8(WR.IX + off, BR.L);	
	  
}


void Alu::LD_L_SRL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.L = doSR(memory->read8(WR.IY + off), 0);
	memory->write8(WR.IY + off, BR.L);	
	  
}


void Alu::LD_R_A()
{
	tstates += 1;
    Register::R = BR.A;
}


void Alu::LD_SP_off_nn()
{
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	WR.SP = memory->read16(addr);	
}


void Alu::LD_SP_HL()
{
	tstates += 2;
	WR.SP = WR.HL;
}


void Alu::LD_SP_IX()
{
	tstates += 2;
	WR.SP = WR.IX;
}


void Alu::LD_SP_IY()
{
	tstates += 2;
	WR.SP = WR.IY;
}


void Alu::LD_SP_nn()
{
	WR.SP = memory->read16(Register::PC);
	Register::PC += 2;
	
}


void Alu::LDD()
{
	tstates += 2;
	byte_t val = memory->read8(WR.HL);
	memory->write8(WR.DE, val);
	WR.DE--;
	WR.HL--;
	WR.BC--;
	valFlag(F_5, ((BR.A + val) & 0x02) != 0);
	valFlag(F_3, ((BR.A + val) & F_3) != 0);
	resFlag(F_H | F_N);
	valFlag(F_PV, WR.BC != 0);
}


void Alu::LDDR()
{
	LDD();
	if (WR.BC != 0)
	{
		tstates += 5;
		Register::PC -= 2;
	}
}


void Alu::LDI()
{
	tstates += 2;
	byte_t val = memory->read8(WR.HL);
	memory->write8(WR.DE, val);
	WR.DE++;
	WR.HL++;
	WR.BC--;
	valFlag(F_5, (BR.A + val) & 0x02);
	valFlag(F_3, ((BR.A + val) & F_3) != 0);
	resFlag(F_H | F_N);
	valFlag(F_PV, WR.BC != 0);
}


void Alu::LDIR()
{
	LDI();
	if (WR.BC != 0)
	{
		tstates += 5;
		Register::PC -= 2;
	}
}


void Alu::NEG()
{
	int temp = BR.A;
	BR.A = 0;
	BR.A = doArithmetic(temp, 0, 1);
	setFlag(F_N);
}


void Alu::NOP()
{
	/* NOP */
	
}


void Alu::OR_off_HL()
{
	doOR(memory->read8(WR.HL));
}


void Alu::OR_off_IX_d()
{
	tstates += 5;
	doOR(memory->read8(WR.IX + (char) memory->read8(Register::PC++)));
}


void Alu::OR_off_IY_d()
{
	tstates += 5;
	doOR(memory->read8(WR.IY + (char) memory->read8(Register::PC++)));
}


void Alu::OR_A()
{
	doOR(BR.A);
}


void Alu::OR_B()
{
	doOR(BR.B);
}


void Alu::OR_C()
{
	doOR(BR.C);
}


void Alu::OR_D()
{
	doOR(BR.D);
}


void Alu::OR_E()
{
	doOR(BR.E);
}


void Alu::OR_H()
{
	doOR(BR.H);
}


void Alu::OR_IXh()
{
	doOR(BR.IXh);
}


void Alu::OR_IXl()
{
	doOR(BR.IXl);
}


void Alu::OR_IYh()
{
	doOR(BR.IYh);
}


void Alu::OR_IYl()
{
	doOR(BR.IYl);
}


void Alu::OR_L()
{
	doOR(BR.L);
}


void Alu::OR_n()
{
	doOR(memory->read8(Register::PC++));
}


void Alu::OTDR()
{
	OUTD();
	if (BR.B != 0)
	{
		tstates += 5;
		Register::PC -= 2;
	}
}


void Alu::OTIR()
{
	OUTI();
	if (BR.B != 0)
	{
		tstates += 5;
		Register::PC -= 2;
	}
}


void Alu::OUT_off_C_0()
{
	ioWrite(WR.BC, 0);
	
}


void Alu::OUT_off_C_A()
{
	ioWrite(WR.BC, BR.A);
}


void Alu::OUT_off_C_B()
{
	ioWrite(WR.BC, BR.B);
}


void Alu::OUT_off_C_C()
{
	ioWrite(WR.BC, BR.C);
}


void Alu::OUT_off_C_D()
{
	ioWrite(WR.BC, BR.D);
}


void Alu::OUT_off_C_E()
{
	ioWrite(WR.BC, BR.E);
}


void Alu::OUT_off_C_H()
{
	ioWrite(WR.BC, BR.H);
}


void Alu::OUT_off_C_L()
{
	ioWrite(WR.BC, BR.L);
}


void Alu::OUT_off_n_A()
{
	ioWrite(BR.A << 8 | memory->read8(Register::PC++), BR.A);
}


void Alu::OUTD()
{
	tstates += 1;
	byte_t value = memory->read8(WR.HL);
	BR.B = doIncDec(BR.B, 1);
	ioWrite(WR.BC, value);
	WR.HL--;
	int flag_value = value + BR.L;
	valFlag(F_N, value & 0x80);
	valFlag(F_H, flag_value > 0xff);
	valFlag(F_C, flag_value > 0xff);
	valFlag(F_PV, parityBit[(flag_value & 7) ^ BR.B]);
	adjustFlags(BR.B);
}


void Alu::OUTI()
{
	tstates += 1;
	byte_t value = memory->read8(WR.HL);
	BR.B = doIncDec(BR.B, 1);
	ioWrite(WR.BC, value);
	WR.HL++;
	int flag_value = value + BR.L;
	valFlag(F_N, value & 0x80);
	valFlag(F_H, flag_value > 0xff);
	valFlag(F_C, flag_value > 0xff);
	valFlag(F_PV, parityBit[(flag_value & 7) ^ BR.B]);
	adjustFlags(BR.B);
}


void Alu::POP_AF()
{
	WR.AF = doPop();
}


void Alu::POP_BC()
{
	WR.BC = doPop();
}


void Alu::POP_DE()
{
	WR.DE = doPop();
}


void Alu::POP_HL()
{
	WR.HL = doPop();
}


void Alu::POP_IX()
{
	WR.IX = doPop();
}


void Alu::POP_IY()
{
	WR.IY = doPop();
}


void Alu::PUSH_AF()
{
	tstates += 1;
	doPush(WR.AF);
}


void Alu::PUSH_BC()
{
	tstates += 1;
	doPush(WR.BC);
}


void Alu::PUSH_DE()
{
	tstates += 1;
	doPush(WR.DE);
}


void Alu::PUSH_HL()
{
	tstates += 1;
	doPush(WR.HL);
}


void Alu::PUSH_IX()
{
	tstates += 1;
	doPush(WR.IX);
}


void Alu::PUSH_IY()
{
	tstates += 1;
	doPush(WR.IY);
}


void Alu::RES_0_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_RES, 0, memory->read8(WR.HL)));
}


void Alu::RES_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_RES, 0, memory->read8(WR.IX + off)));
	
	
}


void Alu::RES_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_RES, 0, memory->read8(WR.IY + off)));
	
	
}


void Alu::RES_0_A()
{
	BR.A = doSetRes(SR_RES, 0, BR.A);
}


void Alu::RES_0_B()
{
	BR.B = doSetRes(SR_RES, 0, BR.B);
}


void Alu::RES_0_C()
{
	BR.C = doSetRes(SR_RES, 0, BR.C);
}


void Alu::RES_0_D()
{
	BR.D = doSetRes(SR_RES, 0, BR.D);
}


void Alu::RES_0_E()
{
	BR.E = doSetRes(SR_RES, 0, BR.E);
}


void Alu::RES_0_H()
{
	BR.H = doSetRes(SR_RES, 0, BR.H);
}


void Alu::RES_0_L()
{
	BR.L = doSetRes(SR_RES, 0, BR.L);
}


void Alu::RES_1_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_RES, 1, memory->read8(WR.HL)));
}


void Alu::RES_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_RES, 1, memory->read8(WR.IX + off)));
	
	
}


void Alu::RES_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_RES, 1, memory->read8(WR.IY + off)));
	
	
}


void Alu::RES_1_A()
{
	BR.A = doSetRes(SR_RES, 1, BR.A);
}


void Alu::RES_1_B()
{
	BR.B = doSetRes(SR_RES, 1, BR.B);
}


void Alu::RES_1_C()
{
	BR.C = doSetRes(SR_RES, 1, BR.C);
}


void Alu::RES_1_D()
{
	BR.D = doSetRes(SR_RES, 1, BR.D);
}


void Alu::RES_1_E()
{
	BR.E = doSetRes(SR_RES, 1, BR.E);
}


void Alu::RES_1_H()
{
	BR.H = doSetRes(SR_RES, 1, BR.H);
}


void Alu::RES_1_L()
{
	BR.L = doSetRes(SR_RES, 1, BR.L);
}


void Alu::RES_2_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_RES, 2, memory->read8(WR.HL)));
}


void Alu::RES_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_RES, 2, memory->read8(WR.IX + off)));
	
	
}


void Alu::RES_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_RES, 2, memory->read8(WR.IY + off)));
	
	
}


void Alu::RES_2_A()
{
	BR.A = doSetRes(SR_RES, 2, BR.A);
}


void Alu::RES_2_B()
{
	BR.B = doSetRes(SR_RES, 2, BR.B);
}


void Alu::RES_2_C()
{
	BR.C = doSetRes(SR_RES, 2, BR.C);
}


void Alu::RES_2_D()
{
	BR.D = doSetRes(SR_RES, 2, BR.D);
}


void Alu::RES_2_E()
{
	BR.E = doSetRes(SR_RES, 2, BR.E);
}


void Alu::RES_2_H()
{
	BR.H = doSetRes(SR_RES, 2, BR.H);
}


void Alu::RES_2_L()
{
	BR.L = doSetRes(SR_RES, 2, BR.L);
}


void Alu::RES_3_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_RES, 3, memory->read8(WR.HL)));
}


void Alu::RES_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_RES, 3, memory->read8(WR.IX + off)));
	
	
}


void Alu::RES_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_RES, 3, memory->read8(WR.IY + off)));
	
	
}


void Alu::RES_3_A()
{
	BR.A = doSetRes(SR_RES, 3, BR.A);
}


void Alu::RES_3_B()
{
	BR.B = doSetRes(SR_RES, 3, BR.B);
}


void Alu::RES_3_C()
{
	BR.C = doSetRes(SR_RES, 3, BR.C);
}


void Alu::RES_3_D()
{
	BR.D = doSetRes(SR_RES, 3, BR.D);
}


void Alu::RES_3_E()
{
	BR.E = doSetRes(SR_RES, 3, BR.E);
}


void Alu::RES_3_H()
{
	BR.H = doSetRes(SR_RES, 3, BR.H);
}


void Alu::RES_3_L()
{
	BR.L = doSetRes(SR_RES, 3, BR.L);
}


void Alu::RES_4_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_RES, 4, memory->read8(WR.HL)));
}


void Alu::RES_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_RES, 4, memory->read8(WR.IX + off)));
	
	
}


void Alu::RES_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_RES, 4, memory->read8(WR.IY + off)));
	
	
}


void Alu::RES_4_A()
{
	BR.A = doSetRes(SR_RES, 4, BR.A);
}


void Alu::RES_4_B()
{
	BR.B = doSetRes(SR_RES, 4, BR.B);
}


void Alu::RES_4_C()
{
	BR.C = doSetRes(SR_RES, 4, BR.C);
}


void Alu::RES_4_D()
{
	BR.D = doSetRes(SR_RES, 4, BR.D);
}


void Alu::RES_4_E()
{
	BR.E = doSetRes(SR_RES, 4, BR.E);
}


void Alu::RES_4_H()
{
	BR.H = doSetRes(SR_RES, 4, BR.H);
}


void Alu::RES_4_L()
{
	BR.L = doSetRes(SR_RES, 4, BR.L);
}


void Alu::RES_5_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_RES, 5, memory->read8(WR.HL)));
}


void Alu::RES_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_RES, 5, memory->read8(WR.IX + off)));
	
	
}


void Alu::RES_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_RES, 5, memory->read8(WR.IY + off)));
	
	
}


void Alu::RES_5_A()
{
	BR.A = doSetRes(SR_RES, 5, BR.A);
}


void Alu::RES_5_B()
{
	BR.B = doSetRes(SR_RES, 5, BR.B);
}


void Alu::RES_5_C()
{
	BR.C = doSetRes(SR_RES, 5, BR.C);
}


void Alu::RES_5_D()
{
	BR.D = doSetRes(SR_RES, 5, BR.D);
}


void Alu::RES_5_E()
{
	BR.E = doSetRes(SR_RES, 5, BR.E);
}


void Alu::RES_5_H()
{
	BR.H = doSetRes(SR_RES, 5, BR.H);
}


void Alu::RES_5_L()
{
	BR.L = doSetRes(SR_RES, 5, BR.L);
}


void Alu::RES_6_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_RES, 6, memory->read8(WR.HL)));
}


void Alu::RES_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_RES, 6, memory->read8(WR.IX + off)));
	
	
}


void Alu::RES_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_RES, 6, memory->read8(WR.IY + off)));
	
	
}


void Alu::RES_6_A()
{
	BR.A = doSetRes(SR_RES, 6, BR.A);
}


void Alu::RES_6_B()
{
	BR.B = doSetRes(SR_RES, 6, BR.B);
}


void Alu::RES_6_C()
{
	BR.C = doSetRes(SR_RES, 6, BR.C);
}


void Alu::RES_6_D()
{
	BR.D = doSetRes(SR_RES, 6, BR.D);
}


void Alu::RES_6_E()
{
	BR.E = doSetRes(SR_RES, 6, BR.E);
}


void Alu::RES_6_H()
{
	BR.H = doSetRes(SR_RES, 6, BR.H);
}


void Alu::RES_6_L()
{
	BR.L = doSetRes(SR_RES, 6, BR.L);
}


void Alu::RES_7_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_RES, 7, memory->read8(WR.HL)));
}


void Alu::RES_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_RES, 7, memory->read8(WR.IX + off)));
	
	
}


void Alu::RES_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_RES, 7, memory->read8(WR.IY + off)));
	
	
}


void Alu::RES_7_A()
{
	BR.A = doSetRes(SR_RES, 7, BR.A);
}


void Alu::RES_7_B()
{
	BR.B = doSetRes(SR_RES, 7, BR.B);
}


void Alu::RES_7_C()
{
	BR.C = doSetRes(SR_RES, 7, BR.C);
}


void Alu::RES_7_D()
{
	BR.D = doSetRes(SR_RES, 7, BR.D);
}


void Alu::RES_7_E()
{
	BR.E = doSetRes(SR_RES, 7, BR.E);
}


void Alu::RES_7_H()
{
	BR.H = doSetRes(SR_RES, 7, BR.H);
}


void Alu::RES_7_L()
{
	BR.L = doSetRes(SR_RES, 7, BR.L);
}


void Alu::RET()
{
	Register::PC = doPop();
	
}


void Alu::RET_C()
{
	tstates += 1;
	if (condition(C_C))
		Register::PC = doPop();
		
}


void Alu::RET_M()
{
	tstates += 1;
	if (condition(C_M))
		Register::PC = doPop();
		
}


void Alu::RET_NC()
{
	tstates += 1;
	if (condition(C_NC))
		Register::PC = doPop();
		
}


void Alu::RET_NZ()
{
	tstates += 1;
	if (condition(C_NZ))
		Register::PC = doPop();
		
}


void Alu::RET_P()
{
	tstates += 1;
	if (condition(C_P))
		Register::PC = doPop();
		
}


void Alu::RET_PE()
{
	tstates += 1;
	if (condition(C_PE))
		Register::PC = doPop();
		
}


void Alu::RET_PO()
{
	tstates += 1;
	if (condition(C_PO))
		Register::PC = doPop();
		
}


void Alu::RET_Z()
{
	tstates += 1;
	if (condition(C_Z))
		Register::PC = doPop();
		
}


void Alu::RETI()
{
	Register::IFF1 = Register::IFF2;
	RET		();
		
}


void Alu::RETN()
{
	Register::IFF1 = Register::IFF2;
	RET();
}


void Alu::RL_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doRL(1, memory->read8(WR.HL)));
	
}


void Alu::RL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doRL(1, memory->read8(WR.IX + off)));
}


void Alu::RL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doRL(1, memory->read8(WR.IY + off)));
}


void Alu::RL_A()
{
	BR.A = doRL(1, BR.A);
}


void Alu::RL_B()
{
	BR.B = doRL(1, BR.B);
}


void Alu::RL_C()
{
	BR.C = doRL(1, BR.C);
}


void Alu::RL_D()
{
	BR.D = doRL(1, BR.D);
}


void Alu::RL_E()
{
	BR.E = doRL(1, BR.E);
}


void Alu::RL_H()
{
	BR.H = doRL(1, BR.H);
}


void Alu::RL_L()
{
	BR.L = doRL(1, BR.L);
}


void Alu::RLA()
{
	BR.A = doRL(0, BR.A);
	
	
}


void Alu::RLC_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doRLC(1, memory->read8(WR.HL)));
	
}


void Alu::RLC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doRLC(1, memory->read8(WR.IX + off)));
}


void Alu::RLC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doRLC(1, memory->read8(WR.IY + off)));
}


void Alu::RLC_A()
{
	BR.A = doRLC(1, BR.A);
}


void Alu::RLC_B()
{
	BR.B = doRLC(1, BR.B);
}


void Alu::RLC_C()
{
	BR.C = doRLC(1, BR.C);
}


void Alu::RLC_D()
{
	BR.D = doRLC(1, BR.D);
}


void Alu::RLC_E()
{
	BR.E = doRLC(1, BR.E);
}


void Alu::RLC_H()
{
	BR.H = doRLC(1, BR.H);
}


void Alu::RLC_L()
{
	BR.L = doRLC(1, BR.L);
}


void Alu::RLCA()
{
	BR.A = doRLC(0, BR.A);
	
	
}


void Alu::RLD()
{
	tstates += 4;
	byte_t Ah = BR.A & 0x0f;
	byte_t hl = memory->read8(WR.HL);
	BR.A = (BR.A & 0xf0) | ((hl & 0xf0) >> 4);
	hl = (hl << 4) | Ah;
	memory->write8(WR.HL, hl);
	resFlag(F_H | F_N);
	adjustFlagSZP(BR.A);
	adjustFlags(BR.A);
}


void Alu::RR_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doRR(1, memory->read8(WR.HL)));
	
}


void Alu::RR_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doRR(1, memory->read8(WR.IX + off)));
}


void Alu::RR_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doRR(1, memory->read8(WR.IY + off)));
}


void Alu::RR_A()
{
	BR.A = doRR(1, BR.A);
}


void Alu::RR_B()
{
	BR.B = doRR(1, BR.B);
}


void Alu::RR_C()
{
	BR.C = doRR(1, BR.C);
}


void Alu::RR_D()
{
	BR.D = doRR(1, BR.D);
}


void Alu::RR_E()
{
	BR.E = doRR(1, BR.E);
}


void Alu::RR_H()
{
	BR.H = doRR(1, BR.H);
}


void Alu::RR_L()
{
	BR.L = doRR(1, BR.L);
}


void Alu::RRA()
{
	BR.A = doRR(0, BR.A);
	
	
}


void Alu::RRC_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doRRC(1, memory->read8(WR.HL)));
	
}


void Alu::RRC_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doRRC(1, memory->read8(WR.IX + off)));
}


void Alu::RRC_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doRRC(1, memory->read8(WR.IY + off)));
}


void Alu::RRC_A()
{
	BR.A = doRRC(1, BR.A);
}


void Alu::RRC_B()
{
	BR.B = doRRC(1, BR.B);
}


void Alu::RRC_C()
{
	BR.C = doRRC(1, BR.C);
}


void Alu::RRC_D()
{
	BR.D = doRRC(1, BR.D);
}


void Alu::RRC_E()
{
	BR.E = doRRC(1, BR.E);
}


void Alu::RRC_H()
{
	BR.H = doRRC(1, BR.H);
}


void Alu::RRC_L()
{
	BR.L = doRRC(1, BR.L);
}


void Alu::RRCA()
{
	BR.A = doRRC(0, BR.A);
	
	
}


void Alu::RRD()
{
	tstates += 4;
	byte_t Ah = BR.A & 0x0f;
	byte_t hl = memory->read8(WR.HL);
	BR.A = (BR.A & 0xf0) | (hl & 0x0f);
	hl = (hl >> 4) | (Ah << 4);
	memory->write8(WR.HL, hl);
	resFlag(F_H | F_N);
	adjustFlagSZP(BR.A);
}


void Alu::RST_0H()
{
	tstates += 1;
	doPush(Register::PC);
	Register::PC = 0x00;
	
	
}


void Alu::RST_10H()
{
	tstates += 1;
	doPush(Register::PC);
	Register::PC = 0x010;
	
	
}


void Alu::RST_18H()
{
	tstates += 1;
	doPush(Register::PC);
	Register::PC = 0x018;
	
	
}


void Alu::RST_20H()
{
	tstates += 1;
	doPush(Register::PC);
	Register::PC = 0x020;
	
	
}


void Alu::RST_28H()
{
	tstates += 1;
	doPush(Register::PC);
	Register::PC = 0x028;
	
	
}


void Alu::RST_30H()
{
	tstates += 1;
	doPush(Register::PC);
	Register::PC = 0x030;
	
	
}


void Alu::RST_38H()
{
	tstates += 1;
	doPush(Register::PC);
	Register::PC = 0x038;
	
	
}


void Alu::RST_8H()
{
	tstates += 1;
	doPush(Register::PC);
	Register::PC = 0x08;
	
	
}


void Alu::SBC_A_off_HL()
{
	BR.A = doArithmetic(memory->read8(WR.HL), F1_SBC, F2_SBC);
}


void Alu::SBC_A_off_IX_d()
{
	tstates += 5;
	char displacement = memory->read8(Register::PC++);
	BR.A = doArithmetic(memory->read8(WR.IX + displacement), F1_SBC, F2_SBC);
	
}


void Alu::SBC_A_off_IY_d()
{
	tstates += 5;
	char displacement = memory->read8(Register::PC++);
	BR.A = doArithmetic(memory->read8(WR.IY + displacement), F1_SBC, F2_SBC);
	
}


void Alu::SBC_A_A()
{
	BR.A = doArithmetic(BR.A, F1_SBC, F2_SBC);
}


void Alu::SBC_A_B()
{
	BR.A = doArithmetic(BR.B, F1_SBC, F2_SBC);
}


void Alu::SBC_A_C()
{
	BR.A = doArithmetic(BR.C, F1_SBC, F2_SBC);
}


void Alu::SBC_A_D()
{
	BR.A = doArithmetic(BR.D, F1_SBC, F2_SBC);
}


void Alu::SBC_A_E()
{
	BR.A = doArithmetic(BR.E, F1_SBC, F2_SBC);
}


void Alu::SBC_A_H()
{
	BR.A = doArithmetic(BR.H, F1_SBC, F2_SBC);
}


void Alu::SBC_A_IXh()
{
	BR.A = doArithmetic(BR.IXh, F1_SBC, F2_SBC);
}


void Alu::SBC_A_IXl()
{
	BR.A = doArithmetic(BR.IXl, F1_SBC, F2_SBC);
}


void Alu::SBC_A_IYh()
{
	BR.A = doArithmetic(BR.IYh, F1_SBC, F2_SBC);
}


void Alu::SBC_A_IYl()
{
	BR.A = doArithmetic(BR.IYl, F1_SBC, F2_SBC);
}


void Alu::SBC_A_L()
{
	BR.A = doArithmetic(BR.L, F1_SBC, F2_SBC);
}


void Alu::SBC_A_n()
{
	BR.A = doArithmetic(memory->read8(Register::PC++), F1_SBC, F2_SBC);
}


void Alu::SBC_HL_BC()
{
	tstates += 7;
	WR.HL = doAddWord(WR.HL, WR.BC, F1_SBC, F2_SBC);
}


void Alu::SBC_HL_DE()
{
	tstates += 7;
	WR.HL = doAddWord(WR.HL, WR.DE, F1_SBC, F2_SBC);
}


void Alu::SBC_HL_HL()
{
	tstates += 7;
	WR.HL = doAddWord(WR.HL, WR.HL, F1_SBC, F2_SBC);
}


void Alu::SBC_HL_SP()
{
	tstates += 7;
	WR.HL = doAddWord(WR.HL, WR.SP, F1_SBC, F2_SBC);
}


void Alu::SCF()
{
	setFlag(F_C);
	resFlag(F_N | F_H);
	adjustFlags(BR.A);
}


void Alu::SET_0_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_SET, 0, memory->read8(WR.HL)));
}


void Alu::SET_0_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_SET, 0, memory->read8(WR.IX + off)));
	
	
}


void Alu::SET_0_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_SET, 0, memory->read8(WR.IY + off)));
	
	
}


void Alu::SET_0_A()
{
	BR.A = doSetRes(SR_SET, 0, BR.A);
}


void Alu::SET_0_B()
{
	BR.B = doSetRes(SR_SET, 0, BR.B);
}


void Alu::SET_0_C()
{
	BR.C = doSetRes(SR_SET, 0, BR.C);
}


void Alu::SET_0_D()
{
	BR.D = doSetRes(SR_SET, 0, BR.D);
}


void Alu::SET_0_E()
{
	BR.E = doSetRes(SR_SET, 0, BR.E);
}


void Alu::SET_0_H()
{
	BR.H = doSetRes(SR_SET, 0, BR.H);
}


void Alu::SET_0_L()
{
	BR.L = doSetRes(SR_SET, 0, BR.L);
}


void Alu::SET_1_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_SET, 1, memory->read8(WR.HL)));
}


void Alu::SET_1_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_SET, 1, memory->read8(WR.IX + off)));
	
	
}


void Alu::SET_1_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_SET, 1, memory->read8(WR.IY + off)));
	
	
}


void Alu::SET_1_A()
{
	BR.A = doSetRes(SR_SET, 1, BR.A);
}


void Alu::SET_1_B()
{
	BR.B = doSetRes(SR_SET, 1, BR.B);
}


void Alu::SET_1_C()
{
	BR.C = doSetRes(SR_SET, 1, BR.C);
}


void Alu::SET_1_D()
{
	BR.D = doSetRes(SR_SET, 1, BR.D);
}


void Alu::SET_1_E()
{
	BR.E = doSetRes(SR_SET, 1, BR.E);
}


void Alu::SET_1_H()
{
	BR.H = doSetRes(SR_SET, 1, BR.H);
}


void Alu::SET_1_L()
{
	BR.L = doSetRes(SR_SET, 1, BR.L);
}


void Alu::SET_2_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_SET, 2, memory->read8(WR.HL)));
}


void Alu::SET_2_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_SET, 2, memory->read8(WR.IX + off)));
	
	
}


void Alu::SET_2_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_SET, 2, memory->read8(WR.IY + off)));
	
	
}


void Alu::SET_2_A()
{
	BR.A = doSetRes(SR_SET, 2, BR.A);
}


void Alu::SET_2_B()
{
	BR.B = doSetRes(SR_SET, 2, BR.B);
}


void Alu::SET_2_C()
{
	BR.C = doSetRes(SR_SET, 2, BR.C);
}


void Alu::SET_2_D()
{
	BR.D = doSetRes(SR_SET, 2, BR.D);
}


void Alu::SET_2_E()
{
	BR.E = doSetRes(SR_SET, 2, BR.E);
}


void Alu::SET_2_H()
{
	BR.H = doSetRes(SR_SET, 2, BR.H);
}


void Alu::SET_2_L()
{
	BR.L = doSetRes(SR_SET, 2, BR.L);
}


void Alu::SET_3_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_SET, 3, memory->read8(WR.HL)));
}


void Alu::SET_3_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_SET, 3, memory->read8(WR.IX + off)));
	
	
}


void Alu::SET_3_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_SET, 3, memory->read8(WR.IY + off)));
	
	
}


void Alu::SET_3_A()
{
	BR.A = doSetRes(SR_SET, 3, BR.A);
}


void Alu::SET_3_B()
{
	BR.B = doSetRes(SR_SET, 3, BR.B);
}


void Alu::SET_3_C()
{
	BR.C = doSetRes(SR_SET, 3, BR.C);
}


void Alu::SET_3_D()
{
	BR.D = doSetRes(SR_SET, 3, BR.D);
}


void Alu::SET_3_E()
{
	BR.E = doSetRes(SR_SET, 3, BR.E);
}


void Alu::SET_3_H()
{
	BR.H = doSetRes(SR_SET, 3, BR.H);
}


void Alu::SET_3_L()
{
	BR.L = doSetRes(SR_SET, 3, BR.L);
}


void Alu::SET_4_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_SET, 4, memory->read8(WR.HL)));
}


void Alu::SET_4_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_SET, 4, memory->read8(WR.IX + off)));
	
	
}


void Alu::SET_4_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_SET, 4, memory->read8(WR.IY + off)));
	
	
}


void Alu::SET_4_A()
{
	BR.A = doSetRes(SR_SET, 4, BR.A);
}


void Alu::SET_4_B()
{
	BR.B = doSetRes(SR_SET, 4, BR.B);
}


void Alu::SET_4_C()
{
	BR.C = doSetRes(SR_SET, 4, BR.C);
}


void Alu::SET_4_D()
{
	BR.D = doSetRes(SR_SET, 4, BR.D);
}


void Alu::SET_4_E()
{
	BR.E = doSetRes(SR_SET, 4, BR.E);
}


void Alu::SET_4_H()
{
	BR.H = doSetRes(SR_SET, 4, BR.H);
}


void Alu::SET_4_L()
{
	BR.L = doSetRes(SR_SET, 4, BR.L);
}


void Alu::SET_5_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_SET, 5, memory->read8(WR.HL)));
}


void Alu::SET_5_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_SET, 5, memory->read8(WR.IX + off)));
	
	
}


void Alu::SET_5_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_SET, 5, memory->read8(WR.IY + off)));
	
	
}


void Alu::SET_5_A()
{
	BR.A = doSetRes(SR_SET, 5, BR.A);
}


void Alu::SET_5_B()
{
	BR.B = doSetRes(SR_SET, 5, BR.B);
}


void Alu::SET_5_C()
{
	BR.C = doSetRes(SR_SET, 5, BR.C);
}


void Alu::SET_5_D()
{
	BR.D = doSetRes(SR_SET, 5, BR.D);
}


void Alu::SET_5_E()
{
	BR.E = doSetRes(SR_SET, 5, BR.E);
}


void Alu::SET_5_H()
{
	BR.H = doSetRes(SR_SET, 5, BR.H);
}


void Alu::SET_5_L()
{
	BR.L = doSetRes(SR_SET, 5, BR.L);
}


void Alu::SET_6_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_SET, 6, memory->read8(WR.HL)));
}


void Alu::SET_6_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_SET, 6, memory->read8(WR.IX + off)));
	
	
}


void Alu::SET_6_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_SET, 6, memory->read8(WR.IY + off)));
	
	
}


void Alu::SET_6_A()
{
	BR.A = doSetRes(SR_SET, 6, BR.A);
}


void Alu::SET_6_B()
{
	BR.B = doSetRes(SR_SET, 6, BR.B);
}


void Alu::SET_6_C()
{
	BR.C = doSetRes(SR_SET, 6, BR.C);
}


void Alu::SET_6_D()
{
	BR.D = doSetRes(SR_SET, 6, BR.D);
}


void Alu::SET_6_E()
{
	BR.E = doSetRes(SR_SET, 6, BR.E);
}


void Alu::SET_6_H()
{
	BR.H = doSetRes(SR_SET, 6, BR.H);
}


void Alu::SET_6_L()
{
	BR.L = doSetRes(SR_SET, 6, BR.L);
}


void Alu::SET_7_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_SET, 7, memory->read8(WR.HL)));
}


void Alu::SET_7_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IX + off, doSetRes(SR_SET, 7, memory->read8(WR.IX + off)));
	
	
}


void Alu::SET_7_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.IY + off, doSetRes(SR_SET, 7, memory->read8(WR.IY + off)));
	
	
}


void Alu::SET_7_A()
{
	BR.A = doSetRes(SR_SET, 7, BR.A);
}


void Alu::SET_7_B()
{
	BR.B = doSetRes(SR_SET, 7, BR.B);
}


void Alu::SET_7_C()
{
	BR.C = doSetRes(SR_SET, 7, BR.C);
}


void Alu::SET_7_D()
{
	BR.D = doSetRes(SR_SET, 7, BR.D);
}


void Alu::SET_7_E()
{
	BR.E = doSetRes(SR_SET, 7, BR.E);
}


void Alu::SET_7_H()
{
	BR.H = doSetRes(SR_SET, 7, BR.H);
}


void Alu::SET_7_L()
{
	BR.L = doSetRes(SR_SET, 7, BR.L);
}


void Alu::SLA_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSL(memory->read8(WR.HL), IA_A));
}


void Alu::SLA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);	
	memory->write8(WR.IX + off, doSL(memory->read8(WR.IX + off), IA_A));
}


void Alu::SLA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);	
	memory->write8(WR.IY + off, doSL(memory->read8(WR.IY + off), IA_A));
}


void Alu::SLA_A()
{
	BR.A = doSL(BR.A, IA_A);
	
}


void Alu::SLA_B()
{
	BR.B = doSL(BR.B, IA_A);
	
}


void Alu::SLA_C()
{
	BR.C = doSL(BR.C, IA_A);
	
}


void Alu::SLA_D()
{
	BR.D = doSL(BR.D, IA_A);
	
}


void Alu::SLA_E()
{
	BR.E = doSL(BR.E, IA_A);
	
}


void Alu::SLA_H()
{
	BR.H = doSL(BR.H, IA_A);
	
}


void Alu::SLA_L()
{
	BR.L = doSL(BR.L, IA_A);
	
}


void Alu::SLL_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSL(memory->read8(WR.HL), IA_L));
}


void Alu::SLL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);	
	memory->write8(WR.IX + off, doSL(memory->read8(WR.IX + off), IA_L));
}


void Alu::SLL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);	
	memory->write8(WR.IY + off, doSL(memory->read8(WR.IY + off), IA_L));
}


void Alu::SLL_A()
{
	BR.A = doSL(BR.A, IA_L);
	
}


void Alu::SLL_B()
{
	BR.B = doSL(BR.B, IA_L);
	
}


void Alu::SLL_C()
{
	BR.C = doSL(BR.C, IA_L);
	
}


void Alu::SLL_D()
{
	BR.D = doSL(BR.D, IA_L);
	
}


void Alu::SLL_E()
{
	BR.E = doSL(BR.E, IA_L);
	
}


void Alu::SLL_H()
{
	BR.H = doSL(BR.H, IA_L);
	
}


void Alu::SLL_L()
{
	BR.L = doSL(BR.L, IA_L);
	
}


void Alu::SRA_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSR(memory->read8(WR.HL), IA_A));
}


void Alu::SRA_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);	
	memory->write8(WR.IX + off, doSR(memory->read8(WR.IX + off), IA_A));
}


void Alu::SRA_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);	
	memory->write8(WR.IY + off, doSR(memory->read8(WR.IY + off), IA_A));
}


void Alu::SRA_A()
{
	BR.A = doSR(BR.A, IA_A);
	
}


void Alu::SRA_B()
{
	BR.B = doSR(BR.B, IA_A);
	
}


void Alu::SRA_C()
{
	BR.C = doSR(BR.C, IA_A);
	
}


void Alu::SRA_D()
{
	BR.D = doSR(BR.D, IA_A);
	
}


void Alu::SRA_E()
{
	BR.E = doSR(BR.E, IA_A);
	
}


void Alu::SRA_H()
{
	BR.H = doSR(BR.H, IA_A);
	
}


void Alu::SRA_L()
{
	BR.L = doSR(BR.L, IA_A);
	
}


void Alu::SRL_off_HL()
{
	tstates += 1;
	memory->write8(WR.HL, doSR(memory->read8(WR.HL), IA_L));
}


void Alu::SRL_off_IX_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);	
	memory->write8(WR.IX + off, doSR(memory->read8(WR.IX + off), IA_L));
}


void Alu::SRL_off_IY_d()
{
	tstates += 2;
	char off = memory->read8(Register::PC++);	
	memory->write8(WR.IY + off, doSR(memory->read8(WR.IY + off), IA_L));
}


void Alu::SRL_A()
{
	BR.A = doSR(BR.A, IA_L);
	
}


void Alu::SRL_B()
{
	BR.B = doSR(BR.B, IA_L);
	
}


void Alu::SRL_C()
{
	BR.C = doSR(BR.C, IA_L);
	
}


void Alu::SRL_D()
{
	BR.D = doSR(BR.D, IA_L);
	
}


void Alu::SRL_E()
{
	BR.E = doSR(BR.E, IA_L);
	
}


void Alu::SRL_H()
{
	BR.H = doSR(BR.H, IA_L);
	
}


void Alu::SRL_L()
{
	BR.L = doSR(BR.L, IA_L);
	
}


void Alu::SUB_A_off_HL()
{
	BR.A = doArithmetic(memory->read8(WR.HL), F1_SUB, F2_SUB);
}


void Alu::SUB_A_off_IX_d()
{
	tstates += 5;
	char displacement = memory->read8(Register::PC++);
	BR.A = doArithmetic(memory->read8(WR.IX + displacement), F1_SUB, F2_SUB);
	
}


void Alu::SUB_A_off_IY_d()
{
	tstates += 5;
	char displacement = memory->read8(Register::PC++);
	BR.A = doArithmetic(memory->read8(WR.IY + displacement), F1_SUB, F2_SUB);
	
}


void Alu::SUB_A_A()
{
	BR.A = doArithmetic(BR.A, F1_SUB, F2_SUB);
}


void Alu::SUB_A_B()
{
	BR.A = doArithmetic(BR.B, F1_SUB, F2_SUB);
}


void Alu::SUB_A_C()
{
	BR.A = doArithmetic(BR.C, F1_SUB, F2_SUB);
}


void Alu::SUB_A_D()
{
	BR.A = doArithmetic(BR.D, F1_SUB, F2_SUB);
}


void Alu::SUB_A_E()
{
	BR.A = doArithmetic(BR.E, F1_SUB, F2_SUB);
}


void Alu::SUB_A_H()
{
	BR.A = doArithmetic(BR.H, F1_SUB, F2_SUB);
}


void Alu::SUB_A_IXh()
{
	BR.A = doArithmetic(BR.IXh, F1_SUB, F2_SUB);
}


void Alu::SUB_A_IXl()
{
	BR.A = doArithmetic(BR.IXl, F1_SUB, F2_SUB);
}


void Alu::SUB_A_IYh()
{
	BR.A = doArithmetic(BR.IYh, F1_SUB, F2_SUB);
}


void Alu::SUB_A_IYl()
{
	BR.A = doArithmetic(BR.IYl, F1_SUB, F2_SUB);
}


void Alu::SUB_A_L()
{
	BR.A = doArithmetic(BR.L, F1_SUB, F2_SUB);
}


void Alu::SUB_A_n()
{
	BR.A = doArithmetic(memory->read8(Register::PC++), F1_SUB, F2_SUB);
}


void Alu::XOR_off_HL()
{
	doXOR(memory->read8(WR.HL));
}


void Alu::XOR_off_IX_d()
{
	tstates += 5;
	doXOR(memory->read8(WR.IX + (char) memory->read8(Register::PC++)));
}


void Alu::XOR_off_IY_d()
{
	tstates += 5;
	doXOR(memory->read8(WR.IY + (char) memory->read8(Register::PC++)));
}


void Alu::XOR_A()
{
	doXOR(BR.A);
}


void Alu::XOR_B()
{
	doXOR(BR.B);
}


void Alu::XOR_C()
{
	doXOR(BR.C);
}


void Alu::XOR_D()
{
	doXOR(BR.D);
}


void Alu::XOR_E()
{
	doXOR(BR.E);
}


void Alu::XOR_H()
{
	doXOR(BR.H);
}


void Alu::XOR_IXh()
{
	doXOR(BR.IXh);
}


void Alu::XOR_IXl()
{
	doXOR(BR.IXl);
}


void Alu::XOR_IYh()
{
	doXOR(BR.IYh);
}


void Alu::XOR_IYl()
{
	doXOR(BR.IYl);
}


void Alu::XOR_L()
{
	doXOR(BR.L);
}


void Alu::XOR_n()
{
	doXOR(memory->read8(Register::PC++));
}


