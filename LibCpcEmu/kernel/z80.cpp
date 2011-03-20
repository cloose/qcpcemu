#include "z80.h"
#include "z80_p.cpp"

#include <QCoreApplication>
#include <QDebug>

#include "exceptions.h"
#include "ioport.h"
#include "logger.h"
#include "memory.h"
#include "registerset.h"
#include "z80_tables.h"


static const byte_t PREFIX_CB = 0xcb;
static const byte_t PREFIX_DD = 0xdd;
static const byte_t PREFIX_ED = 0xed;
static const byte_t PREFIX_FD = 0xfd;


Z80::Z80()
    : m_opCode(0x00)
    , m_cycleCount(0)
    , m_interruptMode(0)
    , m_eiDelay(0)
    , m_interruptPending(false)
    , m_halt(false)
{
    reset();
}

void Z80::registerIoPort(IoPort* port)
{
    m_ioPorts.append(port);
}

void Z80::reset()
{
    // main registers
    REGISTER_AF = 0x0000;
    REGISTER_BC = 0x0000;
    REGISTER_DE = 0x0000;
    REGISTER_HL = 0x0000;

    // shadow registers
    REGISTER_AF1 = 0x0000;
    REGISTER_BC1 = 0x0000;
    REGISTER_DE1 = 0x0000;
    REGISTER_HL1 = 0x0000;

    // index registers
    REGISTER_IX = 0x0000;
    REGISTER_IY = 0x0000;

    REGISTER_PC = 0x0000;
    REGISTER_SP = 0xc000;

    // disable interrupts on reset (Z80 manual  p.23)
    RegisterSet::IFF1 = RegisterSet::IFF2 = 0;

    // after RESET the CPU automatically enters interrupt mode 0 (Z80 manual p.25)
    m_interruptMode = 0;
}

int Z80::step()
{
    m_opCode = fetchInstruction();
    m_cycleCount = Cycles[m_opCode];

    switch (m_opCode)
    {
        case PREFIX_CB:
            m_opCode = fetchInstruction();
            executeOpCodeCB();
            break;

        case PREFIX_DD:
            m_opCode = fetchInstruction();
            executeOpCodeXX(REGISTER_IX);
            break;

        case PREFIX_ED:
            m_opCode = fetchInstruction();
            executeOpCodeED();
            break;

        case PREFIX_FD:
            m_opCode = fetchInstruction();
            executeOpCodeXX(REGISTER_IY);
            break;

        default:
            executeOpCode();
            break;
    }

    return m_cycleCount;
}

void Z80::checkInterrupt()
{
    // after an EI instruction, we need to process
    // one more instruction because of possible RET
    if (m_eiDelay)
    {
        if (--m_eiDelay == 0)
        {
            RegisterSet::IFF1 = RegisterSet::IFF2 = P_FLAG; //???
            // 300 Hz interrupt signal from gate array?
            if (m_interruptPending)
            {
                interruptHandler();
            }
        }
    }
    else if (m_interruptPending)
    {
        interruptHandler();
    }
}

void Z80::setInterruptPending()
{
    m_interruptPending = true;
}

byte_t Z80::fetchInstruction()
{
//    Logger::getInstance()->logProgramCounter(REGISTER_PC);
    return ReadByteFromMemory(REGISTER_PC++);
}

void Z80::interruptHandler()
{
    // interrupts enabled?
    if (RegisterSet::IFF1)
    {
        // automatically reset interrupt flip-flops (Z80 manual p. 23)
        RegisterSet::IFF1 = RegisterSet::IFF2 = 0;

        m_interruptPending = false;

        // get out of HALT instruction
        if( m_halt )
        {
            m_halt = false;
            REGISTER_PC++;
        }

        switch (m_interruptMode)
        {
            case 1:
                Rst(0x0038);
                break;
        }
    }
}

void Z80::executeOpCode()
{
    switch (m_opCode)
    {
        case 0x00: /* nop */        break;

        case 0x01: /* ld bc,nn */   Load(REGISTER_BC, ConstantWord()); break;
        case 0x02: /* ld (bc),a */  Load(MemoryLocationW(REGISTER_BC), REGISTER_A); break;
        case 0x03: /* inc bc */     Inc(REGISTER_BC); break;
        case 0x04: /* inc b */      Inc(REGISTER_B); break;
        case 0x05: /* dec b */      Dec(REGISTER_B); break;
        case 0x06: /* ld b,n */     Load(REGISTER_B, ConstantByte()); break;
        case 0x07: /* rlca */       Rlca(); break;
        case 0x08: /* ex af,af' */  qSwap(REGISTER_AF, REGISTER_AF1); break;
        case 0x09: /* add hl,bc */  Add(REGISTER_HL, REGISTER_BC); break;
        case 0x0a: /* ld a,(bc) */  Load(REGISTER_A, MemoryLocationR(REGISTER_BC)); break;
        case 0x0b: /* dec bc */     Dec(REGISTER_BC); break;
        case 0x0c: /* inc c */      Inc(REGISTER_C); break;
        case 0x0d: /* dec c */      Dec(REGISTER_C); break;
        case 0x0e: /* ld c,n */     Load(REGISTER_C, ConstantByte()); break;
        case 0x0f: /* rrca */       Rrca(); break;

        case 0x10: /* djnz e */
            if( --REGISTER_B )
            {
                m_cycleCount += cc_ex[m_opCode];
                JumpRelative();
            }
            else
            {
                REGISTER_PC++;
            }
            break;
        case 0x11: /* ld de,nn */   Load(REGISTER_DE, ConstantWord()); break;
        case 0x12: /* ld (de),a */  Load(MemoryLocationW(REGISTER_DE), REGISTER_A); break;
        case 0x13: /* inc de */     Inc(REGISTER_DE); break;
        case 0x14: /* inc d */      Inc(REGISTER_D); break;
        case 0x15: /* dec d */      Dec(REGISTER_D); break;
        case 0x16: /* ld d,n */     Load(REGISTER_D, ConstantByte()); break;
        case 0x17: /* rla */        Rla(); break;
        case 0x18: /* jr e */       JumpRelative(); break;
        case 0x19: /* add hl,de */  Add(REGISTER_HL, REGISTER_DE); break;
        case 0x1a: /* ld a,(de) */  Load(REGISTER_A, MemoryLocationR(REGISTER_DE)); break;
        case 0x1b: /* dec de */     Dec(REGISTER_DE); break;
        case 0x1c: /* inc e */      Inc(REGISTER_E); break;
        case 0x1d: /* dec e */      Dec(REGISTER_E); break;
        case 0x1e: /* ld e,n */     Load(REGISTER_E, ConstantByte()); break;
        case 0x1f: /* rra */        Rra(); break;

        case 0x20: /* jr nz,e */
            if (REGISTER_F & Z_FLAG)
            {
                REGISTER_PC++;
            }
            else
            {
                m_cycleCount += cc_ex[m_opCode];
                JumpRelative();
            }
            break;
        case 0x21: /* ld hl,nn */   Load(REGISTER_HL, ConstantWord()); break;
        case 0x22: /* ld (nn),hl */
            // TODO: new LoadXXX() function?
            {
                // retrieve address
                byte_t low  = ReadByteFromMemory(REGISTER_PC++);
                byte_t high = ReadByteFromMemory(REGISTER_PC++);
                word_t address = WORD(low, high);

                // store register content at address
                WriteByteToMemory(address++, LOBYTE(REGISTER_HL));
                WriteByteToMemory(address, HIBYTE(REGISTER_HL));
            }
            break;
        case 0x23: /* inc hl */     Inc(REGISTER_HL); break;
        case 0x24: /* inc h */      Inc(REGISTER_H); break;
        case 0x25: /* dec h */      Dec(REGISTER_H); break;
        case 0x26: /* ld h,n */     Load(REGISTER_H, ConstantByte()); break;
        case 0x27: /* daa */        Daa(); break;
        case 0x28: /* jr z,e */
            if( REGISTER_F & Z_FLAG )
            {
                m_cycleCount += cc_ex[m_opCode];
                JumpRelative();
            }
            else
            {
                REGISTER_PC++;
            }
            break;
        case 0x29: /* add hl,hl */  Add(REGISTER_HL, REGISTER_HL); break;
        case 0x2a: /* ld hl,(nn) */ Load(REGISTER_HL, MemoryLocationWordR(ConstantWord())); break;
        case 0x2b: /* dec hl */     Dec(REGISTER_HL); break;
        case 0x2c: /* inc l */      Inc(REGISTER_L); break;
        case 0x2d: /* dec l */      Dec(REGISTER_L); break;
        case 0x2e: /* ld l,n */     Load(REGISTER_L, ConstantByte()); break;
        case 0x2f: /* cpl */        Cpl(); break;

        case 0x30: /* jr nc,e */
            if (REGISTER_F & C_FLAG)
            {
                REGISTER_PC++;
            }
            else
            {
                m_cycleCount += cc_ex[m_opCode];
                JumpRelative();
            }
            break;
        case 0x31: /* ld sp,nn */   Load(REGISTER_SP, ConstantWord()); break;
        case 0x32: /* ld (nn),a */  Load(MemoryLocationW(ConstantWord()), REGISTER_A); break;
        case 0x33: /* inc sp */     Inc(REGISTER_SP); break;
        case 0x34: /* inc (hl) */
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Inc(value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0x35: /* dec (hl) */
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Dec(value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0x36: /* ld (hl),n */  Load(MemoryLocationW(REGISTER_HL), ConstantByte()); break;
        case 0x37: /* scf */
            SetFlag(C_FLAG);
            ResetFlag(N_FLAG|H_FLAG);
            break;
        case 0x38: /* jr c,e */
            if( REGISTER_F & C_FLAG )
            {
                m_cycleCount += cc_ex[m_opCode];
                JumpRelative();
            }
            else
            {
                REGISTER_PC++;
            }
            break;
        case 0x39: /* add hl,sp */  Add(REGISTER_HL, REGISTER_SP); break;
        case 0x3a: /* ld a,(nn) */  Load(REGISTER_A, MemoryLocationR(ConstantWord())); break;
        case 0x3b: /* dec sp */     Dec(REGISTER_SP); break;
        case 0x3c: /* inc a */      Inc(REGISTER_A); break;
        case 0x3d: /* dec a */      Dec(REGISTER_A); break;
        case 0x3e: /* ld a,n */     Load(REGISTER_A, ConstantByte()); break;
        case 0x3f: /* ccf */
            REGISTER_F ^= C_FLAG;
            ResetFlag(N_FLAG|H_FLAG);
            REGISTER_F |= REGISTER_F & C_FLAG ? 0 : H_FLAG;
            break;

        case 0x40: /* ld b,b */     Load(REGISTER_B, REGISTER_B); break;
        case 0x41: /* ld b,c */     Load(REGISTER_B, REGISTER_C); break;
        case 0x42: /* ld b,d */     Load(REGISTER_B, REGISTER_D); break;
        case 0x43: /* ld b,e */     Load(REGISTER_B, REGISTER_E); break;
        case 0x44: /* ld b,h */     Load(REGISTER_B, REGISTER_H); break;
        case 0x45: /* ld b,l */     Load(REGISTER_B, REGISTER_L); break;
        case 0x46: /* ld b,(hl) */  Load(REGISTER_B, MemoryLocationR(REGISTER_HL)); break;
        case 0x47: /* ld b,a */     Load(REGISTER_B, REGISTER_A); break;

        case 0x48: /* ld c,b */     Load(REGISTER_C, REGISTER_B); break;
        case 0x49: /* ld c,c */     Load(REGISTER_C, REGISTER_C); break;
        case 0x4a: /* ld c,d */     Load(REGISTER_C, REGISTER_D); break;
        case 0x4b: /* ld c,e */     Load(REGISTER_C, REGISTER_E); break;
        case 0x4c: /* ld c,h */     Load(REGISTER_C, REGISTER_H); break;
        case 0x4d: /* ld c,l */     Load(REGISTER_C, REGISTER_L); break;
        case 0x4e: /* ld c,(hl) */  Load(REGISTER_C, MemoryLocationR(REGISTER_HL)); break;
        case 0x4f: /* ld c,a */     Load(REGISTER_C, REGISTER_A); break;

        case 0x50: /* ld d,b */     Load(REGISTER_D, REGISTER_B); break;
        case 0x51: /* ld d,c */     Load(REGISTER_D, REGISTER_C); break;
        case 0x52: /* ld d,d */     Load(REGISTER_D, REGISTER_D); break;
        case 0x53: /* ld d,e */     Load(REGISTER_D, REGISTER_E); break;
        case 0x54: /* ld d,h */     Load(REGISTER_D, REGISTER_H); break;
        case 0x55: /* ld d,l */     Load(REGISTER_D, REGISTER_L); break;
        case 0x56: /* ld d,(hl) */  Load(REGISTER_D, MemoryLocationR(REGISTER_HL)); break;
        case 0x57: /* ld d,a */     Load(REGISTER_D, REGISTER_A); break;

        case 0x58: /* ld e,b */     Load(REGISTER_E, REGISTER_B); break;
        case 0x59: /* ld e,c */     Load(REGISTER_E, REGISTER_C); break;
        case 0x5a: /* ld e,d */     Load(REGISTER_E, REGISTER_D); break;
        case 0x5b: /* ld e,e */     Load(REGISTER_E, REGISTER_E); break;
        case 0x5c: /* ld e,h */     Load(REGISTER_E, REGISTER_H); break;
        case 0x5d: /* ld e,l */     Load(REGISTER_E, REGISTER_L); break;
        case 0x5e: /* ld e,(hl) */  Load(REGISTER_E, MemoryLocationR(REGISTER_HL)); break;
        case 0x5f: /* ld e,a */     Load(REGISTER_E, REGISTER_A); break;

        case 0x60: /* ld h,b */     Load(REGISTER_H, REGISTER_B); break;
        case 0x61: /* ld h,c */     Load(REGISTER_H, REGISTER_C); break;
        case 0x62: /* ld h,d */     Load(REGISTER_H, REGISTER_D); break;
        case 0x63: /* ld h,e */     Load(REGISTER_H, REGISTER_E); break;
        case 0x64: /* ld h,h */     Load(REGISTER_H, REGISTER_H); break;
        case 0x65: /* ld h,l */     Load(REGISTER_H, REGISTER_L); break;
        case 0x66: /* ld h,(hl) */  Load(REGISTER_H, MemoryLocationR(REGISTER_HL)); break;
        case 0x67: /* ld h,a */     Load(REGISTER_H, REGISTER_A); break;

        case 0x68: /* ld l,b */     Load(REGISTER_L, REGISTER_B); break;
        case 0x69: /* ld l,c */     Load(REGISTER_L, REGISTER_C); break;
        case 0x6a: /* ld l,d */     Load(REGISTER_L, REGISTER_D); break;
        case 0x6b: /* ld l,e */     Load(REGISTER_L, REGISTER_E); break;
        case 0x6c: /* ld l,h */     Load(REGISTER_L, REGISTER_H); break;
        case 0x6d: /* ld l,l */     Load(REGISTER_L, REGISTER_L); break;
        case 0x6e: /* ld l,(hl) */  Load(REGISTER_L, MemoryLocationR(REGISTER_HL)); break;
        case 0x6f: /* ld l,a */     Load(REGISTER_L, REGISTER_A); break;

        case 0x70: /* ld (hl),b */  Load(MemoryLocationW(REGISTER_HL), REGISTER_B); break;
        case 0x71: /* ld (hl),c */  Load(MemoryLocationW(REGISTER_HL), REGISTER_C); break;
        case 0x72: /* ld (hl),d */  Load(MemoryLocationW(REGISTER_HL), REGISTER_D); break;
        case 0x73: /* ld (hl),e */  Load(MemoryLocationW(REGISTER_HL), REGISTER_E); break;
        case 0x74: /* ld (hl),h */  Load(MemoryLocationW(REGISTER_HL), REGISTER_H); break;
        case 0x75: /* ld (hl),l */  Load(MemoryLocationW(REGISTER_HL), REGISTER_L); break;
        case 0x76: /* halt */       m_halt = true; REGISTER_PC--; break;
        case 0x77: /* ld (hl),a */  Load(MemoryLocationW(REGISTER_HL), REGISTER_A); break;

        case 0x78: /* ld a,b */     Load(REGISTER_A, REGISTER_B); break;
        case 0x79: /* ld a,c */     Load(REGISTER_A, REGISTER_C); break;
        case 0x7a: /* ld a,d */     Load(REGISTER_A, REGISTER_D); break;
        case 0x7b: /* ld a,e */     Load(REGISTER_A, REGISTER_E); break;
        case 0x7c: /* ld a,h */     Load(REGISTER_A, REGISTER_H); break;
        case 0x7d: /* ld a,l */     Load(REGISTER_A, REGISTER_L); break;
        case 0x7e: /* ld a,(hl) */  Load(REGISTER_A, MemoryLocationR(REGISTER_HL)); break;
        case 0x7f: /* ld a,a */     Load(REGISTER_A, REGISTER_A); break;

        case 0x80: /* add a,b */    Add(REGISTER_B); break;
        case 0x81: /* add a,c */    Add(REGISTER_C); break;
        case 0x82: /* add a,d */    Add(REGISTER_D); break;
        case 0x83: /* add a,e */    Add(REGISTER_E); break;
        case 0x84: /* add a,h */    Add(REGISTER_H); break;
        case 0x85: /* add a,l */    Add(REGISTER_L); break;
        case 0x86: /* add a,(hl) */ Add(MemoryLocationR(REGISTER_HL)); break;
        case 0x87: /* add a,a */    Add(REGISTER_A); break;

        case 0x88: /* adc b */      Adc(REGISTER_B); break;
        case 0x89: /* adc c */      Adc(REGISTER_C); break;
        case 0x8a: /* adc d */      Adc(REGISTER_D); break;
        case 0x8b: /* adc e */      Adc(REGISTER_E); break;
        case 0x8c: /* adc h */      Adc(REGISTER_H); break;
        case 0x8d: /* adc l */      Adc(REGISTER_L); break;
        case 0x8e: /* adc (hl) */   Adc(MemoryLocationR(REGISTER_HL)); break;
        case 0x8f: /* adc a */      Adc(REGISTER_A); break;

        case 0x90: /* sub b */      Sub(REGISTER_B); break;
        case 0x91: /* sub c */      Sub(REGISTER_C); break;
        case 0x92: /* sub d */      Sub(REGISTER_D); break;
        case 0x93: /* sub e */      Sub(REGISTER_E); break;
        case 0x94: /* sub h */      Sub(REGISTER_H); break;
        case 0x95: /* sub l */      Sub(REGISTER_L); break;
        case 0x96: /* sub (hl) */   Sub(MemoryLocationR(REGISTER_HL)); break;
        case 0x97: /* sub a */      Sub(REGISTER_A); break;

        case 0x98: /* sbc b */      Sbc(REGISTER_B); break;
        case 0x99: /* sbc c */      Sbc(REGISTER_C); break;
        case 0x9a: /* sbc d */      Sbc(REGISTER_D); break;
        case 0x9b: /* sbc e */      Sbc(REGISTER_E); break;
        case 0x9c: /* sbc h */      Sbc(REGISTER_H); break;
        case 0x9d: /* sbc l */      Sbc(REGISTER_L); break;
        case 0x9e: /* sbc (hl) */   Sbc(MemoryLocationR(REGISTER_HL)); break;
        case 0x9f: /* sbc a */      Sbc(REGISTER_A); break;

        case 0xa0: /* and b */      And(REGISTER_B); break;
        case 0xa1: /* and c */      And(REGISTER_C); break;
        case 0xa2: /* and d */      And(REGISTER_D); break;
        case 0xa3: /* and e */      And(REGISTER_E); break;
        case 0xa4: /* and h */      And(REGISTER_H); break;
        case 0xa5: /* and l */      And(REGISTER_L); break;
        case 0xa6: /* and (hl) */   And(MemoryLocationR(REGISTER_HL)); break;
        case 0xa7: /* and a */      And(REGISTER_A); break;

        case 0xa8: /* xor b */      Xor(REGISTER_B); break;
        case 0xa9: /* xor c */      Xor(REGISTER_C); break;
        case 0xaa: /* xor d */      Xor(REGISTER_D); break;
        case 0xab: /* xor e */      Xor(REGISTER_E); break;
        case 0xac: /* xor h */      Xor(REGISTER_H); break;
        case 0xad: /* xor l */      Xor(REGISTER_L); break;
        case 0xae: /* xor (hl) */   Xor(MemoryLocationR(REGISTER_HL)); break;
        case 0xaf: /* xor a */      REGISTER_A = 0; REGISTER_F = P_FLAG|Z_FLAG; break;

        case 0xb0: /* or b */       Or(REGISTER_B); break;
        case 0xb1: /* or c */       Or(REGISTER_C); break;
        case 0xb2: /* or d */       Or(REGISTER_D); break;
        case 0xb3: /* or e */       Or(REGISTER_E); break;
        case 0xb4: /* or h */       Or(REGISTER_H); break;
        case 0xb5: /* or l */       Or(REGISTER_L); break;
        case 0xb6: /* or (hl) */    Or(MemoryLocationR(REGISTER_HL)); break;
        case 0xb7: /* or a */       Or(REGISTER_A); break;

        case 0xb8: /* cp b */       Cp(REGISTER_B); break;
        case 0xb9: /* cp c */       Cp(REGISTER_C); break;
        case 0xba: /* cp d */       Cp(REGISTER_D); break;
        case 0xbb: /* cp e */       Cp(REGISTER_E); break;
        case 0xbc: /* cp h */       Cp(REGISTER_H); break;
        case 0xbd: /* cp l */       Cp(REGISTER_L); break;
        case 0xbe: /* cp (hl) */    Cp(MemoryLocationR(REGISTER_HL)); break;
        case 0xbf: /* cp a */       REGISTER_F = N_FLAG|Z_FLAG; break;

        case 0xc0: /* ret nz */
            if (!(REGISTER_F & Z_FLAG))
            {
                m_cycleCount += cc_ex[m_opCode];
                REGISTER_PC = Pop();
            }
            break;
        case 0xc1: /* pop bc */     REGISTER_BC = Pop(); break;
        case 0xc2: /* jp nz,nn */
            if (REGISTER_F & Z_FLAG)
            {
                REGISTER_PC += 2;
            }
            else
            {
                Jump();
            }
            break;
        case 0xc3: /* jp nn */      Jump(); break;
        case 0xc4: /* call nz,nn */
            if (REGISTER_F & Z_FLAG)
            {
                REGISTER_PC += 2;
            }
            else
            {
                m_cycleCount += cc_ex[m_opCode];
                Call();
            }
            break;
        case 0xc5: /* push bc */    Push(REGISTER_BC); break;
        case 0xc6: /* add a,n */    Add(ConstantByte()); break;
        case 0xc7: /* rst 0x00 */   Rst(0x0000); break;
        case 0xc8: /* ret z */
            if (REGISTER_F & Z_FLAG)
            {
                m_cycleCount += cc_ex[m_opCode];
                REGISTER_PC = Pop();
            }
            break;
        case 0xc9: /* ret */        REGISTER_PC = Pop(); break;
        case 0xca: /* jp z,nn */
            if (REGISTER_F & Z_FLAG)
            {
                Jump();
            }
            else
            {
                REGISTER_PC += 2;
            }
            break;
        /* PREFIX 0xcb */
        case 0xcc: /* call z,nn */
            if (REGISTER_F & Z_FLAG)
            {
                m_cycleCount += cc_ex[m_opCode];
                Call();
            }
            else
            {
                REGISTER_PC += 2;
            }
            break;
        case 0xcd: /* call nn */    Call(); break;
        case 0xce: /* adc a,n */    Adc(ConstantByte()); break;
        case 0xcf: /* rst 0x08 */   Rst(0x0008); break;

        case 0xd0: /* ret nc */
            if (!(REGISTER_F & C_FLAG))
            {
                m_cycleCount += cc_ex[m_opCode];
                REGISTER_PC = Pop();
            }
            break;
        case 0xd1: /* pop de */     REGISTER_DE = Pop(); break;
        case 0xd2: /* jp nc,nn */
            if (REGISTER_F & C_FLAG)
            {
                REGISTER_PC += 2;
            }
            else
            {
                Jump();
            }
            break;
        case 0xd3: /* out (n),a */
            emitOutputRequest(WORD(ConstantByte(), REGISTER_A), REGISTER_A);
            break;
        case 0xd4: /* call nc,nn */
            if (REGISTER_F & C_FLAG)
            {
                REGISTER_PC += 2;
            }
            else
            {
                m_cycleCount += cc_ex[m_opCode];
                Call();
            }
            break;
        case 0xd5: /* push de */    Push(REGISTER_DE); break;
        case 0xd6: /* sub n */      Sub(ConstantByte()); break;
        case 0xd7: /* rst 0x10 */   Rst(0x0010); break;
        case 0xd8: /* ret c */
            if (REGISTER_F & C_FLAG)
            {
                m_cycleCount += cc_ex[m_opCode];
                REGISTER_PC = Pop();
            }
            break;
        case 0xd9: /* exx */        Exx(); break;
        case 0xda: /* jp c,nn */
            if (REGISTER_F & C_FLAG)
            {
                Jump();
            }
            else
            {
                REGISTER_PC += 2;
            }
            break;
        case 0xdb: /* in a,(n) */
            REGISTER_A = emitInputRequest(WORD(ConstantByte(), REGISTER_A));
            break;
        case 0xdc: /* call c,nn */
            if (REGISTER_F & C_FLAG)
            {
                m_cycleCount += cc_ex[m_opCode];
                Call();
            }
            else
            {
                REGISTER_PC += 2;
            }
            break;
        /* PREFIX 0xdd */
        case 0xde: /* sbc a,n */    Sbc(ConstantByte()); break;
        case 0xdf: /* rst 0x18 */   Rst(0x0018); break;

        case 0xe0: /* ret po */
            if (!(REGISTER_F & P_FLAG))
            {
                m_cycleCount += cc_ex[m_opCode];
                REGISTER_PC = Pop();
            }
            break;
        case 0xe1: /* pop hl */     REGISTER_HL = Pop(); break;
        case 0xe2: /* jp po,nn */
            if (REGISTER_F & P_FLAG)
            {
                REGISTER_PC += 2;
            }
            else
            {
                Jump();
            }
            break;
        case 0xe3: /* ex (sp),hl */
             {
                 quint8 low  = ReadByteFromMemory(REGISTER_SP);
                 WriteByteToMemory(REGISTER_SP++, REGISTER_L);

                 quint8 high = ReadByteFromMemory(REGISTER_SP);
                 WriteByteToMemory(REGISTER_SP--, REGISTER_H);

                 REGISTER_HL = WORD(low, high);
             }
             break;
        case 0xe4: /* call po,nn */
            if (REGISTER_F & P_FLAG)
            {
                REGISTER_PC += 2;
            }
            else
            {
                m_cycleCount += cc_ex[m_opCode];
                Call();
            }
            break;
        case 0xe5: /* push hl */    Push(REGISTER_HL); break;
        case 0xe6: /* and n */      And(ConstantByte()); break;
        case 0xe7: /* rst 0x20 */   Rst(0x0020); break;
        case 0xe8: /* ret pe */
            if (REGISTER_F & P_FLAG)
            {
                m_cycleCount += cc_ex[m_opCode];
                REGISTER_PC = Pop();
            }
            break;
        case 0xe9: /* jp (hl) */    REGISTER_PC = REGISTER_HL; break;
        case 0xea: /* jp pe,nn */
            if (REGISTER_F & P_FLAG)
            {
                Jump();
            }
            else
            {
                REGISTER_PC += 2;
            }
            break;
        case 0xeb: /* ex de,hl */   qSwap(REGISTER_DE, REGISTER_HL); break;
        case 0xec: /* call pe,nn */
            if (REGISTER_F & P_FLAG)
            {
                m_cycleCount += cc_ex[m_opCode];
                Call();
            }
            else
            {
                REGISTER_PC += 2;
            }
            break;
        /* PREFIX 0xed */
        case 0xee: /* xor n */      Xor(ConstantByte()); break;
        case 0xef: /* rst 0x28 */   Rst(0x0028); break;

        case 0xf0: /* ret p */
            if (!(REGISTER_F & S_FLAG))
            {
                m_cycleCount += cc_ex[m_opCode];
                REGISTER_PC = Pop();
            }
            break;
        case 0xf1: /* pop af */     REGISTER_AF = Pop(); break;
        case 0xf2: /* jp p,nn */
            if (REGISTER_F & S_FLAG)
            {
                REGISTER_PC += 2;
            }
            else
            {
                Jump();
            }
            break;
        case 0xf3: /* di */         RegisterSet::IFF1 = RegisterSet::IFF2 = 0; m_eiDelay = 0; break;
        case 0xf4: /* call p,nn */
            if (REGISTER_F & S_FLAG)
            {
                REGISTER_PC += 2;
            }
            else
            {
                m_cycleCount += cc_ex[m_opCode];
                Call();
            }
            break;
        case 0xf5: /* push af */    Push(REGISTER_AF); break;
        case 0xf6: /* or n */       Or(ConstantByte()); break;
        case 0xf7: /* rst 0x30 */   Rst(0x0030); break;
        case 0xf8: /* ret m */
            if (REGISTER_F & S_FLAG)
            {
                m_cycleCount += cc_ex[m_opCode];
                REGISTER_PC = Pop();
            }
            break;
        case 0xf9: /* ld sp,hl */   Load(REGISTER_SP, REGISTER_HL); break;
        case 0xfa: /* jp m,nn */
            if (REGISTER_F & S_FLAG)
            {
                Jump();
            }
            else
            {
                REGISTER_PC += 2;
            }
            break;
        case 0xfb: /* ei */         RegisterSet::IFF1 = RegisterSet::IFF2 = 1; m_eiDelay = 2; break;
        case 0xfc: /* call m,nn */
            if (REGISTER_F & S_FLAG)
            {
                m_cycleCount += cc_ex[m_opCode];
                Call();
            }
            else
            {
                REGISTER_PC += 2;
            }
            break;
        /* PREFIX 0xfd */
        case 0xfe: /* cp n */       Cp(ConstantByte()); break;
        case 0xff: /* rst 0x38 */   Rst(0x0038); break;

        default:
            qCritical() << "[Z80 ] unhandled opcode" << hex << m_opCode << "at PC" << REGISTER_PC-1;
            throw NotImplementedException("unhandled opcode");
            break;
    }
}

void Z80::executeOpCodeCB()
{
    switch (m_opCode)
    {
        case 0x00: /* rlc b */      Rlc(REGISTER_B); break;
        case 0x01: /* rlc c */      Rlc(REGISTER_C); break;
        case 0x02: /* rlc d */      Rlc(REGISTER_D); break;
        case 0x03: /* rlc e */      Rlc(REGISTER_E); break;
        case 0x04: /* rlc h */      Rlc(REGISTER_H); break;
        case 0x05: /* rlc l */      Rlc(REGISTER_L); break;
        case 0x06: /* rlc (hl) */
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Rlc(value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0x07: /* rlc a */      Rlc(REGISTER_A); break;

        case 0x08: /* rrc b */      Rrc(REGISTER_B); break;
        case 0x09: /* rrc c */      Rrc(REGISTER_C); break;
        case 0x0a: /* rrc d */      Rrc(REGISTER_D); break;
        case 0x0b: /* rrc e */      Rrc(REGISTER_E); break;
        case 0x0c: /* rrc h */      Rrc(REGISTER_H); break;
        case 0x0d: /* rrc l */      Rrc(REGISTER_L); break;

        case 0x10: /* rl b */       Rl(REGISTER_B); break;
        case 0x11: /* rl c */       Rl(REGISTER_C); break;
        case 0x12: /* rl d */       Rl(REGISTER_D); break;
        case 0x13: /* rl e */       Rl(REGISTER_E); break;
        case 0x14: /* rl h */       Rl(REGISTER_H); break;
        case 0x15: /* rl l */       Rl(REGISTER_L); break;
        case 0x16: /* rl (hl) */
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Rl(value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0x17: /* rl a */       Rl(REGISTER_A); break;

        case 0x18: Rr(REGISTER_B); break;
        case 0x19: Rr(REGISTER_C); break;
        case 0x1a: Rr(REGISTER_D); break;
        case 0x1b: Rr(REGISTER_E); break;
        case 0x1c: Rr(REGISTER_H); break;
        case 0x1d: Rr(REGISTER_L); break;
        case 0x1e: /* rr (hl) */
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Rr(value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0x1f: Rr(REGISTER_A);  break;

        case 0x20: Sla(REGISTER_B); break;
        case 0x21: Sla(REGISTER_C); break;
        case 0x22: Sla(REGISTER_D); break;
        case 0x23: Sla(REGISTER_E); break;
        case 0x24: Sla(REGISTER_H); break;
        case 0x25: Sla(REGISTER_L); break;
        case 0x26: /* sla (hl) */
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Sla(value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0x27: Sla(REGISTER_A); break;

        case 0x28: Sra(REGISTER_B); break;
        case 0x29: Sra(REGISTER_C); break;
        case 0x2a: Sra(REGISTER_D); break;
        case 0x2b: Sra(REGISTER_E); break;
        case 0x2c: Sra(REGISTER_H); break;
        case 0x2d: Sra(REGISTER_L); break;
        case 0x2e: /* sra (hl) */
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Sra(value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0x2f: Sra(REGISTER_A); break;

        case 0x38: /* srl b */      Srl(REGISTER_B); break;
        case 0x39: /* srl c */      Srl(REGISTER_C); break;
        case 0x3a: /* srl d */      Srl(REGISTER_D); break;
        case 0x3b: /* srl e */      Srl(REGISTER_E); break;
        case 0x3c: /* srl h */      Srl(REGISTER_H); break;
        case 0x3d: /* srl l */      Srl(REGISTER_L); break;
        case 0x3e: /* srl (hl) */
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Srl(value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0x3f: /* srl a */      Srl(REGISTER_A); break;

        case 0x40: Bit(0, REGISTER_B); break;
        case 0x41: Bit(0, REGISTER_C); break;
        case 0x42: Bit(0, REGISTER_D); break;
        case 0x43: Bit(0, REGISTER_E); break;
        case 0x44: Bit(0, REGISTER_H); break;
        case 0x45: Bit(0, REGISTER_L); break;
        case 0x46: Bit(0, MemoryLocationR(REGISTER_HL)); break;
        case 0x47: Bit(0, REGISTER_A); break;

        case 0x48: Bit(1, REGISTER_B); break;
        case 0x49: Bit(1, REGISTER_C); break;
        case 0x4a: Bit(1, REGISTER_D); break;
        case 0x4b: Bit(1, REGISTER_E); break;
        case 0x4c: Bit(1, REGISTER_H); break;
        case 0x4d: Bit(1, REGISTER_L); break;
        case 0x4e: Bit(1, MemoryLocationR(REGISTER_HL)); break;
        case 0x4f: Bit(1, REGISTER_A); break;

        case 0x50: Bit(2, REGISTER_B); break;
        case 0x51: Bit(2, REGISTER_C); break;
        case 0x52: Bit(2, REGISTER_D); break;
        case 0x53: Bit(2, REGISTER_E); break;
        case 0x54: Bit(2, REGISTER_H); break;
        case 0x55: Bit(2, REGISTER_L); break;
        case 0x56: Bit(2, MemoryLocationR(REGISTER_HL)); break;
        case 0x57: Bit(2, REGISTER_A); break;

        case 0x58: Bit(3, REGISTER_B); break;
        case 0x59: Bit(3, REGISTER_C); break;
        case 0x5a: Bit(3, REGISTER_D); break;
        case 0x5b: Bit(3, REGISTER_E); break;
        case 0x5c: Bit(3, REGISTER_H); break;
        case 0x5d: Bit(3, REGISTER_L); break;
        case 0x5e: Bit(3, MemoryLocationR(REGISTER_HL)); break;
        case 0x5f: Bit(3, REGISTER_A); break;

        case 0x60: Bit(4, REGISTER_B); break;
        case 0x61: Bit(4, REGISTER_C); break;
        case 0x62: Bit(4, REGISTER_D); break;
        case 0x63: Bit(4, REGISTER_E); break;
        case 0x64: Bit(4, REGISTER_H); break;
        case 0x65: Bit(4, REGISTER_L); break;
        case 0x66: Bit(4, MemoryLocationR(REGISTER_HL)); break;
        case 0x67: Bit(4, REGISTER_A); break;

        case 0x68: Bit(5, REGISTER_B); break;
        case 0x69: Bit(5, REGISTER_C); break;
        case 0x6a: Bit(5, REGISTER_D); break;
        case 0x6b: Bit(5, REGISTER_E); break;
        case 0x6c: Bit(5, REGISTER_H); break;
        case 0x6d: Bit(5, REGISTER_L); break;
        case 0x6e: Bit(5, MemoryLocationR(REGISTER_HL)); break;
        case 0x6f: Bit(5, REGISTER_A); break;

        case 0x70: Bit(6, REGISTER_B); break;
        case 0x71: Bit(6, REGISTER_C); break;
        case 0x72: Bit(6, REGISTER_D); break;
        case 0x73: Bit(6, REGISTER_E); break;
        case 0x74: Bit(6, REGISTER_H); break;
        case 0x75: Bit(6, REGISTER_L); break;
        case 0x76: Bit(6, MemoryLocationR(REGISTER_HL)); break;
        case 0x77: Bit(6, REGISTER_A); break;

        case 0x78: Bit(7, REGISTER_B); break;
        case 0x79: Bit(7, REGISTER_C); break;
        case 0x7a: Bit(7, REGISTER_D); break;
        case 0x7b: Bit(7, REGISTER_E); break;
        case 0x7c: Bit(7, REGISTER_H); break;
        case 0x7d: Bit(7, REGISTER_L); break;
        case 0x7e: Bit(7, MemoryLocationR(REGISTER_HL)); break;
        case 0x7f: Bit(7, REGISTER_A); break;

        case 0x80: Res(0, REGISTER_B); break;
        case 0x81: Res(0, REGISTER_C); break;
        case 0x82: Res(0, REGISTER_D); break;
        case 0x83: Res(0, REGISTER_E); break;
        case 0x84: Res(0, REGISTER_H); break;
        case 0x85: Res(0, REGISTER_L); break;
        case 0x86:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Res(0, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0x87: Res(0, REGISTER_A); break;

        case 0x88: Res(1, REGISTER_B); break;
        case 0x89: Res(1, REGISTER_C); break;
        case 0x8a: Res(1, REGISTER_D); break;
        case 0x8b: Res(1, REGISTER_E); break;
        case 0x8c: Res(1, REGISTER_H); break;
        case 0x8d: Res(1, REGISTER_L); break;
        case 0x8e:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Res(1, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0x8f: Res(1, REGISTER_A); break;

        case 0x90: Res(2, REGISTER_B); break;
        case 0x91: Res(2, REGISTER_C); break;
        case 0x92: Res(2, REGISTER_D); break;
        case 0x93: Res(2, REGISTER_E); break;
        case 0x94: Res(2, REGISTER_H); break;
        case 0x95: Res(2, REGISTER_L); break;
        case 0x96:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Res(2, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0x97: Res(2, REGISTER_A); break;

        case 0x98: Res(3, REGISTER_B); break;
        case 0x99: Res(3, REGISTER_C); break;
        case 0x9a: Res(3, REGISTER_D); break;
        case 0x9b: Res(3, REGISTER_E); break;
        case 0x9c: Res(3, REGISTER_H); break;
        case 0x9d: Res(3, REGISTER_L); break;
        case 0x9e:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Res(3, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0x9f: Res(3, REGISTER_A); break;

        case 0xa0: Res(4, REGISTER_B); break;
        case 0xa1: Res(4, REGISTER_C); break;
        case 0xa2: Res(4, REGISTER_D); break;
        case 0xa3: Res(4, REGISTER_E); break;
        case 0xa4: Res(4, REGISTER_H); break;
        case 0xa5: Res(4, REGISTER_L); break;
        case 0xa6:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Res(4, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0xa7: Res(4, REGISTER_A); break;

        case 0xa8: Res(5, REGISTER_B); break;
        case 0xa9: Res(5, REGISTER_C); break;
        case 0xaa: Res(5, REGISTER_D); break;
        case 0xab: Res(5, REGISTER_E); break;
        case 0xac: Res(5, REGISTER_H); break;
        case 0xad: Res(5, REGISTER_L); break;
        case 0xae:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Res(5, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0xaf: Res(5, REGISTER_A); break;

        case 0xb0: Res(6, REGISTER_B); break;
        case 0xb1: Res(6, REGISTER_C); break;
        case 0xb2: Res(6, REGISTER_D); break;
        case 0xb3: Res(6, REGISTER_E); break;
        case 0xb4: Res(6, REGISTER_H); break;
        case 0xb5: Res(6, REGISTER_L); break;
        case 0xb6:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Res(6, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0xb7: Res(6, REGISTER_A); break;

        case 0xb8: Res(7, REGISTER_B); break;
        case 0xb9: Res(7, REGISTER_C); break;
        case 0xba: Res(7, REGISTER_D); break;
        case 0xbb: Res(7, REGISTER_E); break;
        case 0xbc: Res(7, REGISTER_H); break;
        case 0xbd: Res(7, REGISTER_L); break;
        case 0xbe:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Res(7, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0xbf: Res(7, REGISTER_A); break;

        case 0xc0: Set(0, REGISTER_B); break;
        case 0xc1: Set(0, REGISTER_C); break;
        case 0xc2: Set(0, REGISTER_D); break;
        case 0xc3: Set(0, REGISTER_E); break;
        case 0xc4: Set(0, REGISTER_H); break;
        case 0xc5: Set(0, REGISTER_L); break;
        case 0xc6:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Set(0, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0xc7: Set(0, REGISTER_A); break;

        case 0xc8: Set(1, REGISTER_B); break;
        case 0xc9: Set(1, REGISTER_C); break;
        case 0xca: Set(1, REGISTER_D); break;
        case 0xcb: Set(1, REGISTER_E); break;
        case 0xcc: Set(1, REGISTER_H); break;
        case 0xcd: Set(1, REGISTER_L); break;
        case 0xce:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Set(1, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0xcf: Set(1, REGISTER_A); break;

        case 0xd0: Set(2, REGISTER_B); break;
        case 0xd1: Set(2, REGISTER_C); break;
        case 0xd2: Set(2, REGISTER_D); break;
        case 0xd3: Set(2, REGISTER_E); break;
        case 0xd4: Set(2, REGISTER_H); break;
        case 0xd5: Set(2, REGISTER_L); break;
        case 0xd6:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Set(2, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0xd7: Set(2, REGISTER_A); break;

        case 0xd8: Set(3, REGISTER_B); break;
        case 0xd9: Set(3, REGISTER_C); break;
        case 0xda: Set(3, REGISTER_D); break;
        case 0xdb: Set(3, REGISTER_E); break;
        case 0xdc: Set(3, REGISTER_H); break;
        case 0xdd: Set(3, REGISTER_L); break;
        case 0xde:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Set(3, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0xdf: Set(3, REGISTER_A); break;

        case 0xe0: Set(4, REGISTER_B); break;
        case 0xe1: Set(4, REGISTER_C); break;
        case 0xe2: Set(4, REGISTER_D); break;
        case 0xe3: Set(4, REGISTER_E); break;
        case 0xe4: Set(4, REGISTER_H); break;
        case 0xe5: Set(4, REGISTER_L); break;
        case 0xe6:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Set(4, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0xe7: Set(4, REGISTER_A); break;

        case 0xe8: Set(5, REGISTER_B); break;
        case 0xe9: Set(5, REGISTER_C); break;
        case 0xea: Set(5, REGISTER_D); break;
        case 0xeb: Set(5, REGISTER_E); break;
        case 0xec: Set(5, REGISTER_H); break;
        case 0xed: Set(5, REGISTER_L); break;
        case 0xee:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Set(5, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0xef: Set(5, REGISTER_A); break;

        case 0xf0: Set(6, REGISTER_B); break;
        case 0xf1: Set(6, REGISTER_C); break;
        case 0xf2: Set(6, REGISTER_D); break;
        case 0xf3: Set(6, REGISTER_E); break;
        case 0xf4: Set(6, REGISTER_H); break;
        case 0xf5: Set(6, REGISTER_L); break;
        case 0xf6:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Set(6, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0xf7: Set(6, REGISTER_A); break;

        case 0xf8: Set(7, REGISTER_B); break;
        case 0xf9: Set(7, REGISTER_C); break;
        case 0xfa: Set(7, REGISTER_D); break;
        case 0xfb: Set(7, REGISTER_E); break;
        case 0xfc: Set(7, REGISTER_H); break;
        case 0xfd: Set(7, REGISTER_L); break;
        case 0xfe:
            {
                byte_t value = ReadByteFromMemory(REGISTER_HL);
                Set(7, value);
                WriteByteToMemory(REGISTER_HL, value);
            }
            break;
        case 0xff: Set(7, REGISTER_A); break;

        default:
            qCritical() << "[Z80 ] unhandled opcode 0xcb" << hex << m_opCode << "at PC" << REGISTER_PC-2;
            throw NotImplementedException("unhandled opcode");
            break;
    }
}

void Z80::executeOpCodeED()
{
    switch (m_opCode)
    {
        case 0x42: /* sbc hl,bc */  Sbc(REGISTER_HL, REGISTER_BC); break;
        case 0x43: /* ld (nn),bc */
            {
                word_t address = ConstantWord();
                Load(MemoryLocationW(address++), LOBYTE(REGISTER_BC));
                Load(MemoryLocationW(address), HIBYTE(REGISTER_BC));
            }
            break;
        case 0x46: /* im 0 */       m_interruptMode = 0; break;
        case 0x48: /* in c,(c) */
            REGISTER_C = emitInputRequest(REGISTER_BC);
            REGISTER_F = (REGISTER_F & C_FLAG)
                       | SignAndZeroTable[REGISTER_C]
                       | ParityTable[REGISTER_C];
            break;
        case 0x49: /* out (c),c */  emitOutputRequest(REGISTER_BC, REGISTER_C); break;
        case 0x4b: /* ld bc,(nn) */ Load(REGISTER_BC, MemoryLocationWordR(ConstantWord())); break;
        case 0x51: /* out (c),d */  emitOutputRequest(REGISTER_BC, REGISTER_D); break;
        case 0x52: /* sbc hl,de */  Sbc(REGISTER_HL, REGISTER_DE); break;
        case 0x53: /* ld (nn),de */
            // TODO: new LoadXXX() function?
            {
                // retrieve address
                byte_t low  = ReadByteFromMemory(REGISTER_PC++);
                byte_t high = ReadByteFromMemory(REGISTER_PC++);
                word_t address = WORD(low, high);

                // store register content at address
                WriteByteToMemory(address++, LOBYTE(REGISTER_DE));
                WriteByteToMemory(address, HIBYTE(REGISTER_DE));
            }
            break;
        case 0x56: /* im 1 */       m_interruptMode = 1; break;
        case 0x59: /* out (c),e */  emitOutputRequest(REGISTER_BC, REGISTER_E); break;
        case 0x5b: /* ld de,(nn) */ Load(REGISTER_DE, MemoryLocationWordR(ConstantWord())); break;
        case 0x5e: /* im 2 */       m_interruptMode = 2; break;
        case 0x61: /* out (c),h */  emitOutputRequest(REGISTER_BC, REGISTER_H); break;
        case 0x62: /* sbc hl,hl */  Sbc(REGISTER_HL, REGISTER_HL); break;
        case 0x69: /* out (c),l */  emitOutputRequest(REGISTER_BC, REGISTER_L); break;
        case 0x73: /* ld (nn),sp */
            // TODO: new LoadXXX() function?
            {
                // retrieve address
                byte_t low  = ReadByteFromMemory(REGISTER_PC++);
                byte_t high = ReadByteFromMemory(REGISTER_PC++);
                word_t address = WORD(low, high);

                // store register content at address
                WriteByteToMemory(address++, LOBYTE(REGISTER_SP));
                WriteByteToMemory(address, HIBYTE(REGISTER_SP));
            }
            break;
        case 0x78: /* in a,(c) */
            REGISTER_A = emitInputRequest(REGISTER_BC);
            REGISTER_F = (REGISTER_F & C_FLAG)
                       | SignAndZeroTable[REGISTER_A]
                       | ParityTable[REGISTER_A];
            break;
        case 0x79: /* out (c),a */  emitOutputRequest(REGISTER_BC, REGISTER_A); break;
        case 0x7b: /* ld sp,(nn) */ Load(REGISTER_SP, MemoryLocationWordR(ConstantWord())); break;
        case 0xa0: /* ldi */        Ldi(); break;
        case 0xa8: /* ldd */        Ldd(); break;
        case 0xb0: /* ldir */
            Ldi();

            if (REGISTER_BC != 0)
            {
                m_cycleCount += cc_ex[m_opCode];
                REGISTER_PC -= 2;
            }
            break;
        case 0xb8: /* lddr */
            Ldd();

            if (REGISTER_BC)
            {
                m_cycleCount += cc_ex[m_opCode];
                REGISTER_PC -= 2;
            }
            break;

        default:
            qCritical() << "[Z80 ] unhandled opcode 0xed" << hex << m_opCode << "at PC" << REGISTER_PC-2;
            throw NotImplementedException("unhandled opcode");
            break;
    }
}

void Z80::executeOpCodeXX(word_t& destinationRegister)
{
    switch (m_opCode)
    {
        case 0x09: /* add ix,bc */  Add(destinationRegister, REGISTER_BC); break;
        case 0x19: /* add ix,de */  Add(destinationRegister, REGISTER_DE); break;
        case 0x21: /* ld ix,nn */   Load(destinationRegister, ConstantWord()); break;
        case 0x22: /* ld (nn),ix */
            {
                word_t address = ConstantWord();
                Load(MemoryLocationW(address++), LOBYTE(destinationRegister));
                Load(MemoryLocationW(address), HIBYTE(destinationRegister));
            }
            break;
        case 0x23: /* inc ix */     Inc(destinationRegister); break;
        case 0x2a: /* ld ix,(nn) */ Load(destinationRegister, MemoryLocationWordR(ConstantWord())); break;
        case 0x34: /* inc (ix+d) */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                byte_t value = ReadByteFromMemory(destinationRegister+offset);
                Inc(value);
                WriteByteToMemory(destinationRegister+offset, value);
            }
            break;
        case 0x35: /* dec (ix+d) */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                byte_t value = ReadByteFromMemory(destinationRegister+offset);
                Dec(value);
                WriteByteToMemory(destinationRegister+offset, value);
            }
            break;
        case 0x36: /* ld (ix+d),n */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                WriteByteToMemory(destinationRegister+offset, ConstantByte());
            }
            break;
        case 0x39: /* add ix,sp */  Add(destinationRegister, REGISTER_SP); break;
        case 0x4e: /* ld c,(ix+d) */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                REGISTER_C = ReadByteFromMemory(destinationRegister+offset);
            }
            break;
        case 0x56: /* ld d,(ix+d) */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                REGISTER_D = ReadByteFromMemory(destinationRegister+offset);
            }
            break;
        case 0x5e: /* ld e,(ix+d) */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                REGISTER_E = ReadByteFromMemory(destinationRegister+offset);
            }
            break;
        case 0x66: /* ld h,(ix+d) */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                REGISTER_H = ReadByteFromMemory(destinationRegister+offset);
            }
            break;
        case 0x6e: /* ld l,(ix+d) */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                REGISTER_L = ReadByteFromMemory(destinationRegister+offset);
            }
            break;
        case 0x70: /* ld (ix+d),b */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                WriteByteToMemory(destinationRegister+offset, REGISTER_B);
            }
            break;
        case 0x71: /* ld (ix+d),c */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                WriteByteToMemory(destinationRegister+offset, REGISTER_C);
            }
            break;
        case 0x72: /* ld (ix+d),d */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                WriteByteToMemory(destinationRegister+offset, REGISTER_D);
            }
            break;
        case 0x73: /* ld (ix+d),e */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                WriteByteToMemory(destinationRegister+offset, REGISTER_E);
            }
            break;
        case 0x74: /* ld (ix+d),h */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                WriteByteToMemory(destinationRegister+offset, REGISTER_H);
            }
            break;
        case 0x75: /* ld (ix+d),l */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                WriteByteToMemory(destinationRegister+offset, REGISTER_L);
            }
            break;
        case 0x77: /* ld (ix+d),a */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                WriteByteToMemory(destinationRegister+offset, REGISTER_A);
            }
            break;
        case 0x7e: /* ld a,(ix+d) */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                REGISTER_A = ReadByteFromMemory(destinationRegister+offset);
            }
            break;
        case 0x86: /* add a,(ix+d) */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                byte_t value = ReadByteFromMemory(destinationRegister+offset);
                Add(value);
            }
            break;
        case 0x8e: /* adc a,(ix+d) */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                byte_t value = ReadByteFromMemory(destinationRegister+offset);
                Adc(value);
            }
            break;
        case 0xb6: /* or (ix+d) */
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                Or(ReadByteFromMemory(destinationRegister+offset));
            }
            break;
        case 0xbe: // cp (ix+d)
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                byte_t value = ReadByteFromMemory(destinationRegister+offset);
                Cp(value);
            }
            break;
        case PREFIX_CB:
            {
                offset_t offset = static_cast<offset_t>(ConstantByte());
                word_t address = destinationRegister+offset;

                m_opCode = ConstantByte();
                executeOpCodeXXCB(address);
            }
            break;
        case 0xe1: /* pop ix */     destinationRegister = Pop(); break;
        case 0xe5: /* push ix */    Push(destinationRegister); break;
        default:
            qCritical() << "[Z80 ] unhandled opcode 0xdd/0xfd" << hex << m_opCode << "at PC" << REGISTER_PC-2;
            throw NotImplementedException("unhandled opcode");
            break;
    }
}

void Z80::executeOpCodeXXCB(word_t address)
{
    switch (m_opCode)
    {
        case 0x46: /* bit 0,(ix+d) */ Bit(0, MemoryLocationR(address)); break;
        case 0x4e: /* bit 1,(ix+d) */ Bit(1, MemoryLocationR(address)); break;
        case 0x56: /* bit 2,(ix+d) */ Bit(2, MemoryLocationR(address)); break;
        case 0x5e: /* bit 3,(ix+d) */ Bit(3, MemoryLocationR(address)); break;
        case 0x66: /* bit 4,(ix+d) */ Bit(4, MemoryLocationR(address)); break;
        case 0x6e: /* bit 5,(ix+d) */ Bit(5, MemoryLocationR(address)); break;
        case 0x76: /* bit 6,(ix+d) */ Bit(6, MemoryLocationR(address)); break;
        case 0x7e: /* bit 7,(ix+d) */ Bit(7, MemoryLocationR(address)); break;

        case 0xc6: // set 0,(ix+d)
            {
                byte_t value = ReadByteFromMemory(address);
                Set(0, value);
                WriteByteToMemory(address, value);
            }
            break;
        case 0xce: // set 1,(ix+d)
            {
                byte_t value = ReadByteFromMemory(address);
                Set(1, value);
                WriteByteToMemory(address, value);
            }
            break;
        case 0xd6: // set 2,(ix+d)
            {
                byte_t value = ReadByteFromMemory(address);
                Set(2, value);
                WriteByteToMemory(address, value);
            }
            break;
        case 0xde: // set 3,(ix+d)
            {
                byte_t value = ReadByteFromMemory(address);
                Set(3, value);
                WriteByteToMemory(address, value);
            }
            break;
        case 0xe6: // set 4,(ix+d)
            {
                byte_t value = ReadByteFromMemory(address);
                Set(4, value);
                WriteByteToMemory(address, value);
            }
            break;
        case 0xee: // set 5,(ix+d)
            {
                byte_t value = ReadByteFromMemory(address);
                Set(5, value);
                WriteByteToMemory(address, value);
            }
            break;
        case 0xf6: // set 6,(ix+d)
            {
                byte_t value = ReadByteFromMemory(address);
                Set(6, value);
                WriteByteToMemory(address, value);
            }
            break;
        case 0xfe: // set 7,(ix+d)
            {
                byte_t value = ReadByteFromMemory(address);
                Set(7, value);
                WriteByteToMemory(address, value);
            }
            break;
        default:
            qCritical() << "[Z80 ] unhandled opcode 0xdd/0xfd 0xcb" << hex << m_opCode << "at PC" << REGISTER_PC-2;
            throw NotImplementedException("unhandled opcode");
            break;
    }
}

byte_t Z80::emitInputRequest(word_t address)
{
    byte_t result = 0x00;
    bool handled = false;

    foreach (IoPort* port, m_ioPorts)
    {
        handled = port->in(address, result);
        //if (port->in(address, result))
        if (handled)
            break;
    }

    if (!handled)
        throw NotImplementedException(QString("unhandled IN request for address %1").arg(address, 0, 16).toStdString());

    return result;
}

void Z80::emitOutputRequest(word_t address, byte_t value)
{
    bool handled = false;

    foreach (IoPort* port, m_ioPorts)
    {
        handled = port->out(address, value);
//        if (port->out(address, value))
        if (handled)
            break;
    }

    if (!handled && address != 0xef7f && address != 0xf8ff)
        throw NotImplementedException(QString("unhandled OUT request for address %1").arg(address, 0, 16).toStdString());
}
