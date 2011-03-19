#include "logger.h"

#include <QDebug>


Logger::Logger()
{
}

Logger* Logger::getInstance()
{
    static Logger instance;
    return &instance;
}

void Logger::logProgramCounter(word_t pc)
{
    switch (pc)
    {
        case 0x0000: qDebug() << "[Z80  ] ***** RST0: System Reset"; break;
        case 0x0008: qDebug() << "[Z80  ] ***** RST1: LO JUMP"; break;
        case 0x0010: qDebug() << "[Z80  ] ***** RST2: LO SIDE CALL"; break;
        case 0x0018: qDebug() << "[Z80  ] ***** RST3: LO FAR CALL"; break;
        case 0x0020: qDebug() << "[Z80  ] ***** RST4: RAM LAM"; break;
        case 0x0028: qDebug() << "[Z80  ] ***** RST5: FIRM JUMP"; break;
        case 0x0030: qDebug() << "[Z80  ] ***** RST6: USER"; break;
        case 0x0038: qDebug() << "[Z80  ] ***** RST7: INTERRUPT"; break;
        case 0x0044: qDebug() << "[Z80  ] ***** Restore Hi Kernel Jumps"; break;
        case 0x005c: qDebug() << "[Z80  ] ***** KL CHOKE OFF"; break;
        case 0x0099: qDebug() << "[Z80  ] ***** KL TIME PLEASE"; break;
        case 0x00b1: qDebug() << "[Z80  ] ***** Scan Events"; break;
    }
}
