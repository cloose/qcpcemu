#
# Z80 -> C specification


#
# Add / Sub / Adc / Sbc
#
(ADC|SBC|ADD|SUB) A,(A|B|C|D|E|H|L|IXh|IXl|IYh|IYl)
	BR.A = doArithmetic(BR.%2, F1_%1, F2_%1);

(ADC|SBC|ADD|SUB) A,\(HL\)
	BR.A = doArithmetic(memory->read8(WR.HL), F1_%1, F2_%1);

(ADC|SBC|ADD|SUB) A,\((IX|IY)\+d\)
	tstates += 5;
	char displacement = memory->read8(Register::PC++);
	BR.A = doArithmetic(memory->read8(WR.%2 + displacement), F1_%1, F2_%1);
	
(ADC|SBC|ADD|SUB) A,n
	BR.A = doArithmetic(memory->read8(Register::PC++), F1_%1, F2_%1);


# Particular cases
(ADC|ADD|SBC) HL,(SP|BC|DE|HL)
	tstates += 7;
	WR.HL = doAddWord(WR.HL, WR.%2, F1_%1, F2_%1);

ADD (IX|IY),(SP|BC|DE|IX|IY)
	tstates += 7;
	WR.%1 = doAddWord(WR.%1, WR.%2, 0, 0);

#
# Logic operations
#
(AND|XOR|OR) \(HL\)
	do%1(memory->read8(WR.HL));

(AND|XOR|OR) \((IX|IY)\+d\)
	tstates += 5;
	do%1(memory->read8(WR.%2 + (char) memory->read8(Register::PC++)));

(AND|XOR|OR) (A|B|C|D|E|H|L|IXh|IXl|IYh|IYl)
	do%1(BR.%2);

(AND|XOR|OR) n
	do%1(memory->read8(Register::PC++));


#
# Bit operations
#
BIT ([0-7]),(A|B|C|D|E|H|L)
	doBIT_r(%1, BR.%2);

BIT ([0-7]),\(HL\)
	tstates += 1;
	doBIT_r(%1, memory->read8(WR.HL));

BIT ([0-7]),\((IX|IY)\+d\)
	tstates += 2;
	ushort address = WR.%2 + (char) memory->read8(Register::PC++);
	doBIT_indexed(%1, address);

(SET|RES) ([0-7]),(A|B|C|D|E|H|L)
	BR.%3 = doSetRes(SR_%1, %2, BR.%3);

(SET|RES) ([0-7]),\(HL\)
	tstates += 1;
	memory->write8(WR.HL, doSetRes(SR_%1, %2, memory->read8(WR.HL)));

(SET|RES) ([0-7]),\((IX|IY)\+d\)
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.%3 + off, doSetRes(SR_%1, %2, memory->read8(WR.%3 + off)));
	
	
#
# Jumps and calls
#
CALL (C|M|NZ|NC|P|PE|PO|Z)?,?\(nn\)
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_%1))
	{
		tstates += 1;
		doPush(Register::PC);
		Register::PC = addr;
	}
	
JP \((HL|IX|IY)\)
	Register::PC = WR.%1;
	
JP (C|M|NZ|NC|P|PE|PO|Z)?,?\(nn\)
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	if (condition(C_%1))
		Register::PC = addr;
	
JR (C|NZ|NC|Z)?,?\(PC\+e\)
	int off = doComplement(memory->read8(Register::PC++));
	if (condition(C_%1))
	{
		tstates += 5;
		Register::PC += off;
	}

RETI
	Register::IFF1 = Register::IFF2;
	%RET		
		
RETN
	Register::IFF1 = Register::IFF2;
	%RET


RET (C|M|NZ|NC|P|PE|PO|Z)
	tstates += 1;
	if (condition(C_%1))
		Register::PC = doPop();
		
RET
	Register::PC = doPop();

	
DJNZ \(PC\+e\)
	tstates += 1;
	char off = memory->read8(Register::PC++);
	BR.B--;
	if (BR.B)
	{
		tstates += 5;
		Register::PC += off;
	}


RST (0|8|10|18|20|28|30|38)H
	tstates += 1;
	doPush(Register::PC);
	Register::PC = 0x0%1;
	
	
#
# Misc
#
CCF
	valFlag(F_C, (1 - (byte_t)getFlag(F_C) != 0));
	resFlag(F_N);
	adjustFlags(BR.A);

SCF
	setFlag(F_C);
	resFlag(F_N | F_H);
	adjustFlags(BR.A);

CPL
	BR.A = ~BR.A;
	setFlag(F_H | F_N);
	adjustFlags(BR.A);
	
DAA
	doDAA();
	
EX \(SP\),(HL|IX|IY)
	tstates += 3;
	ushort tmp = memory->read16(WR.SP);
	memory->write16(WR.SP, WR.%1);
	WR.%1 = tmp;

EX AF,AF'
	ushort tmp = WR.AF;
	WR.AF = Register::R2.wr.AF;
	Register::R2.wr.AF = tmp;

EX DE,HL
	ushort tmp = WR.DE;
	WR.DE = WR.HL;
	WR.HL = tmp;

EXX
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

HALT
	halted = true;
	Register::PC--;

#
# Compare
#
CP \(HL\)
	doCP_HL();

CP \((IX|IY)\+d\)
	tstates += 5;
	char displacement = memory->read8(Register::PC++);
	byte_t val = memory->read8(WR.%1 + displacement);
	doArithmetic(val, 0, 1);	
	adjustFlags(val);

CP (A|B|C|D|E|H|L|IXh|IXl|IYh|IYl)
	doArithmetic(BR.%1, 0, 1);	
	adjustFlags(BR.%1);

CP n
	byte_t val = memory->read8(Register::PC++);
	doArithmetic(val, 0, 1);	
	adjustFlags(val);

CPDR
	%CPD
	if (WR.BC != 0 && !getFlag(F_Z))
	{
		tstates += 5;
		Register::PC -= 2;
	}

CPD
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

CPIR
	%CPI
	if (WR.BC != 0 && !getFlag(F_Z))
	{
		tstates += 5;
		Register::PC -= 2;
	}

CPI
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

#
# INC and DEC
#
(INC|DEC) \(HL\)
	tstates += 1;
	byte_t value = memory->read8(WR.HL);
	memory->write8(WR.HL, doIncDec(value, ID_%1));

(INC|DEC) \((IX|IY)\+d\)
	tstates += 6;
	char off = memory->read8(Register::PC++);
	byte_t value = memory->read8(WR.%2 + off);
	memory->write8(WR.%2 + off, doIncDec(value, ID_%1));
	
(INC|DEC) (A|B|C|D|E|H|L|IXh|IXl|IYh|IYl)
	BR.%2 = doIncDec(BR.%2, ID_%1);

INC (BC|DE|HL|SP|IX|IY)
	tstates += 2;
	WR.%1++;

DEC (BC|DE|HL|SP|IX|IY)
	tstates += 2;
	WR.%1--;

#
# Interrupts
#
(EI|DI)
	Register::IFF1 = Register::IFF2 = IE_%1;
	deferInterrupt = true;

IM ([012])
	interruptMode = %1;


#
# IO ports
#
IN (A|B|C|D|E|F|H|L),\(C\)
	BR.%1 = ioRead(WR.BC);
	resFlag(F_H | F_N);
	adjustFlagSZP(BR.%1);
	adjustFlags(BR.%1);

IN A,\(n\)
	byte_t port = memory->read8(Register::PC++);	
	BR.A = ioRead(BR.A << 8 | port);

INDR
	%IND
	if (BR.B != 0)
	{
		tstates += 5;
		Register::PC -= 2;
	}

IND
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

INIR
	%INI
	if (BR.B != 0)
	{
		tstates += 5;
		Register::PC -= 2;
	}

INI
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

#
# Loads
#
LD \((BC|DE|HL)\),A
	memory->write8(WR.%1, BR.A);

LD \(HL\),(B|C|D|E|H|L)
	memory->write8(WR.HL, BR.%1);

LD \(HL\),n
	memory->write8(WR.HL, memory->read8(Register::PC++));
	
LD \((IX|IY)\+d\),(A|B|C|D|E|H|L)
	tstates += 5;
	memory->write8(WR.%1 + (char) memory->read8(Register::PC++), BR.%2);
	
LD \((IX|IY)\+d\),n
	tstates += 2;
	char offset = memory->read8(Register::PC++);
	byte_t n = memory->read8(Register::PC++);
	memory->write8(WR.%1 + offset, n);
	
LD \(nn\),A
	memory->write8(memory->read16(Register::PC), BR.A);
	Register::PC += 2;
	
LD (BC|DE|HL|IX|IY|SP),(BC|DE|HL|IX|IY|SP)
	tstates += 2;
	WR.%1 = WR.%2;

LD \(nn\),(BC|DE|HL|IX|IY|SP)
	memory->write16(memory->read16(Register::PC), WR.%1);
	Register::PC += 2;
	
LD A,\((BC|DE)\)
	BR.A = memory->read8(WR.%1);

LD (A|B|C|D|E|H|L),\(HL\)
	BR.%1 = memory->read8(WR.HL);

LD (A|B|C|D|E|H|L),\((IX|IY)\+d\)
	tstates += 5;
	BR.%1 = memory->read8(WR.%2 + (char) memory->read8(Register::PC++));

LD (A|B|C|D|E|H|L),\(nn\)
	BR.%1 = memory->read8(memory->read16(Register::PC));
	Register::PC += 2;
	
LD (A|B|C|D|E|H|L|IXh|IXl|IYh|IYl),(A|B|C|D|E|H|L|IXh|IXl|IYh|IYl)
	BR.%1 = BR.%2;

LD (A|B|C|D|E|H|L),(SL|SR)A \((IX|IY)\+d\)
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.%1 = do%2(memory->read8(WR.%3 + off), 1);
	memory->write8(WR.%3 + off, BR.%1);	
	
LD (A|B|C|D|E|H|L),(SL|SR)L \((IX|IY)\+d\)
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.%1 = do%2(memory->read8(WR.%3 + off), 0);
	memory->write8(WR.%3 + off, BR.%1);	
	  
LD (A|B|C|D|E|H|L),(RL|RLC|RR|RRC) \((IX|IY)\+d\)
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.%1 = do%2(1, memory->read8(WR.%3 + off));
	memory->write8(WR.%3 + off, BR.%1);

LD (A|B|C|D|E|H|L),(SET|RES) ([0-7]),\((IX|IY)\+d\)
	tstates += 2;
	char off = memory->read8(Register::PC++);
	BR.%1 = doSetRes(SR_%2, %3, memory->read8(WR.%4 + off));
	memory->write8(WR.%4 + off, BR.%1);	

LD A,(I|R)
	tstates += 1;
	BR.A = Register::%1;
	adjustFlags(BR.A);
	resFlag(F_H | F_N);
	valFlag(F_PV, Register::IFF2);
	valFlag(F_S, (BR.A & 0x80) != 0);
	valFlag(F_Z, (BR.A == 0));
	
LD (I|R),A
	tstates += 1;
	Register::%1 = BR.A;

LD (A|B|C|D|E|H|L|IXh|IXl|IYh|IYl),n
	BR.%1 = memory->read8(Register::PC++);
	
LD (BC|DE|HL|SP|IX|IY),\(nn\)
	ushort addr = memory->read16(Register::PC);
	Register::PC += 2;
	WR.%1 = memory->read16(addr);	

LD (BC|DE|HL|SP|IX|IY),nn
	WR.%1 = memory->read16(Register::PC);
	Register::PC += 2;
	

LDIR
	%LDI
	if (WR.BC != 0)
	{
		tstates += 5;
		Register::PC -= 2;
	}

LDI
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

LDDR
	%LDD
	if (WR.BC != 0)
	{
		tstates += 5;
		Register::PC -= 2;
	}

LDD
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

NEG
	int temp = BR.A;
	BR.A = 0;
	BR.A = doArithmetic(temp, 0, 1);
	setFlag(F_N);

NOP
	/* NOP */
	

OUTI
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

OTIR
	%OUTI
	if (BR.B != 0)
	{
		tstates += 5;
		Register::PC -= 2;
	}

OUTD
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

OTDR
	%OUTD
	if (BR.B != 0)
	{
		tstates += 5;
		Register::PC -= 2;
	}

OUT \(C\),0
	ioWrite(WR.BC, 0);
	
OUT \(C\),(A|B|C|D|E|H|L)
	ioWrite(WR.BC, BR.%1);

OUT \(n\),A
	ioWrite(BR.A << 8 | memory->read8(Register::PC++), BR.A);

POP (AF|BC|DE|HL|IX|IY)
	WR.%1 = doPop();

PUSH (AF|BC|DE|HL|IX|IY)
	tstates += 1;
	doPush(WR.%1);


#
# Rotate & shift
#

(RLC|RRC|RL|RR) \(HL\)
	tstates += 1;
	memory->write8(WR.HL, do%1(1, memory->read8(WR.HL)));
	
(RLC|RRC|RL|RR) (A|B|C|D|E|H|L|IXh|IXl|IYh|IYl)
	BR.%2 = do%1(1, BR.%2);

(RLC|RRC|RL|RR) \((IX|IY)\+d\)
	tstates += 2;
	char off = memory->read8(Register::PC++);
	memory->write8(WR.%2 + off, do%1(1, memory->read8(WR.%2 + off)));

(RL|RR|RLC|RRC)A
	BR.A = do%1(0, BR.A);
	
	
RLD
	tstates += 4;
	byte_t Ah = BR.A & 0x0f;
	byte_t hl = memory->read8(WR.HL);
	BR.A = (BR.A & 0xf0) | ((hl & 0xf0) >> 4);
	hl = (hl << 4) | Ah;
	memory->write8(WR.HL, hl);
	resFlag(F_H | F_N);
	adjustFlagSZP(BR.A);
	adjustFlags(BR.A);


RRD
	tstates += 4;
	byte_t Ah = BR.A & 0x0f;
	byte_t hl = memory->read8(WR.HL);
	BR.A = (BR.A & 0xf0) | (hl & 0x0f);
	hl = (hl >> 4) | (Ah << 4);
	memory->write8(WR.HL, hl);
	resFlag(F_H | F_N);
	adjustFlagSZP(BR.A);


(SL|SR)(L|A) \(HL\)
	tstates += 1;
	memory->write8(WR.HL, do%1(memory->read8(WR.HL), IA_%2));


(SL|SR)(L|A) \((IX|IY)\+d\)
	tstates += 2;
	char off = memory->read8(Register::PC++);	
	memory->write8(WR.%3 + off, do%1(memory->read8(WR.%3 + off), IA_%2));

(SL|SR)(L|A) (A|B|C|D|E|H|L|IXh|IXl|IYh|IYl)
	BR.%3 = do%1(BR.%3, IA_%2);
	
