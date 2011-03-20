#ifndef FLOPPYCONTROLLER_H
#define FLOPPYCONTROLLER_H

#include "ioport.h"
#include <QtCore/qstack.h>
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

    union FdcCommand
    {
        struct
        {
            byte_t code : 5;
            byte_t sk : 1;
            byte_t mf : 1;
            byte_t mt : 1;
        };

        byte_t value;
    };

    struct DataIterator
    {
        ulong start;
        long length;
        int pointer;

        bool atEnd() const { return pointer == length; }
    };

    byte_t readDataRegister();
    void writeDataRegister(byte_t value);

    void selectDrive();
    void interpretCommandCode();

    void transitToCommandPhase();
    void transitToExecutionPhase();
    void transitToResultPhase();

    void executeCommand();

    void read();
    void readId();
    void seek(uint track);

    Phases m_phase;

    // status of all disk drive motors.
    // (It's not possible to turn on/off a specific motor separately)
    bool m_motorsActive;

    // 8-bit main status register
    byte_t m_mainStatusRegister;
    byte_t m_st0, m_st1, m_st2;

    FdcCommand m_command;
    QVector<byte_t> m_commandParameters;

    QStack<byte_t> m_resultStack;

    FloppyDiskDrive* m_selectedDrive;
    FloppyDiskDrive* m_drives[4];

    DataIterator m_iterator;
};

#endif // FLOPPYCONTROLLER_H
