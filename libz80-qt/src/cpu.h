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

#ifndef Z80_CPU_H
#define Z80_CPU_H

#include "iinputoutput.h"
#include <QtCore/qlist.h>

class IInputOutput;
class IMemory;
class IoPort;

namespace Z80 {

class Alu;

class Cpu : public IInputOutput
{
public:
    explicit Cpu(IMemory *memory);
    virtual ~Cpu();

    void registerIoPort(IoPort* port);
    byte_t emitInputRequest(word_t address);
    void emitOutputRequest(word_t address, byte_t value);

    unsigned execute();
    unsigned executeTStates(unsigned tstates);

    void reset();

    void requestInterrupt(byte_t value = 0);
    void requestNMI();

private:
    Alu *alu;
//    bool nmiRequested;
//    bool interruptRequested;
//    byte_t interruptVector;
    QList<IoPort*> ioPorts;
};

} // namespace Z80

#endif // Z80_CPU_H
