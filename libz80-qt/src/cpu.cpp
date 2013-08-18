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

#include "cpu.h"
using namespace Z80;

#include <QDebug>

#include "alu.h"
#include "ioport.h"
#include "registers.h"

Cpu::Cpu(IMemory *memory) :
    alu(new Alu(memory, this))
{
    reset();
}

void Cpu::registerIoPort(IoPort *port)
{
    ioPorts.append(port);
}

byte_t Cpu::emitInputRequest(word_t address)
{
    byte_t result = 0x00;
    bool handled = false;

    foreach(IoPort * port, ioPorts) {
        handled = port->in(address, result);
        if (handled)
            break;
    }

    if (!handled)
        qFatal("unhandled IN request");
//        throw NotImplementedException(QString("unhandled IN request for address %1").arg(address, 0, 16).toStdString());

    return result;
}

void Cpu::emitOutputRequest(word_t address, byte_t value)
{
    bool handled = false;

    foreach(IoPort * port, ioPorts) {
        handled = port->out(address, value);
        if (handled)
            break;
    }

//    if (!handled && address != 0xef7f && address != 0xf8ff)
//        throw NotImplementedException(QString("unhandled OUT request for address %1").arg(address, 0, 16).toStdString());
}

unsigned Cpu::execute()
{
    return alu->execute();
}

unsigned Cpu::executeTStates(unsigned tstates)
{
    return alu->executeTStates(tstates);
}

void Cpu::reset()
{
    alu->reset();
}

void Cpu::requestInterrupt(byte_t value)
{
    alu->requestInterrupt(value);
}

void Cpu::requestNMI()
{
    alu->requestNMI();
}
