/*
 * Copyright 2013 Christian Loose <christian.loose@hamburg.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef Z80_REGISTERS_H
#define Z80_REGISTERS_H

#include "types.h"

namespace Z80 {

union RegisterSet
{
    // Word registers.
    struct
    {
        word_t AF, BC, DE, HL, IX, IY, SP;
    } wr;

    // Byte registers.
    struct
    {
        byte_t F, A, C, B, E, D, L, H, IXl, IXh, IYl, IYh;
    } br;
};


struct Register
{
    static RegisterSet R1;
    static RegisterSet R2;

    static word_t PC;               // 16-bit Program Counter register

    static byte_t I;                // 8-bit Interrupt Vector
    static byte_t R;                // 8-bit Memory Refresh

    /* interrupt flip-flops */
    static byte_t IFF1;             // IFF1: used to disable interrupts
    static byte_t IFF2;             // IFF2: temporary storage for IFF1
};

#define WR (Register::R1.wr)
#define BR (Register::R1.br)

} // namespace Z80

#endif // Z80_REGISTERS_H
