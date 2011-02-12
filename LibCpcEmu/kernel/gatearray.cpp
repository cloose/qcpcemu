#include "gatearray.h"

#include <QDebug>

#include "memory.h"


GateArray::GateArray(Memory* memory)
    : m_memory(memory)
{
}

bool GateArray::in(word_t address, byte_t& value)
{
    qDebug() << "[GA ] IN request at address" << hex << address;

    return false;
}

bool GateArray::out(word_t address, byte_t value)
{
    qDebug() << "[GA ] OUT request at address" << hex << address << "with value" << hex << value;

    return false;
}
