#ifndef FLOPPYCONTROLLER_H
#define FLOPPYCONTROLLER_H

#include "ioport.h"
#include <QtCore/qvector.h>

class FloppyDiskDrive;


class FloppyController : public IoPort
{
public:
    FloppyController();

    void attachDiskDrive(uint number, FloppyDiskDrive* drive);

    virtual bool in(word_t address, byte_t& value);
    virtual bool out(word_t address, byte_t value);

private:
    enum Phases { CommandPhase, ExecutionPhase, ResultPhase };

    void writeDataRegister(byte_t value);

    void interpretCommandCode();

    void transitToCommandPhase();
    void transitToExecutionPhase();
    void transitToResultPhase();

    void executeCommand();

    Phases m_phase;

    // status of all disk drive motors.
    // (It's not possible to turn on/off a specific motor separately)
    bool m_motorsActive;

    // 8-bit main status register
    byte_t m_mainStatusRegister;

    byte_t m_command;
    QVector<byte_t> m_commandParameters;

    FloppyDiskDrive* m_drives[4];
};

#endif // FLOPPYCONTROLLER_H
