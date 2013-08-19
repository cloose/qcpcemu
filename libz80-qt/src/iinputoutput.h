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

#ifndef IINPUTOUTPUT_H
#define IINPUTOUTPUT_H

#include "types.h"

class IInputOutput
{
public:
    virtual byte_t emitInputRequest(word_t address) = 0;
    virtual void emitOutputRequest(word_t address, byte_t value) = 0;
};

#endif // IINPUTOUTPUT_H
