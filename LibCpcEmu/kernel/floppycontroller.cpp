#include "floppycontroller.h"

#include <QDebug>

enum
{
    BUSY = 0x10,    // b4: FDC Busy
    EXM = 0x20,     // b5: Execution Mode
    DIO = 0x40,     // b6: Data Input/Output
    RQM = 0x80      // b7: Request for Master
};

enum FdcCommands
{
    ReadDiagnosticCommand       = 0x02,
    SpecifyCommand              = 0x03,
    SenseDriveStatusCommand     = 0x04,
    WriteDataCommand            = 0x05,
    ReadDataCommand             = 0x06,
    RecalibrateCommand          = 0x07,
    SenseInterruptStatusCommand = 0x08,
    WriteDeletedDataCommand     = 0x09,
    ReadIdCommand               = 0x0a,
    ReadDeletedDataCommand      = 0x0c,
    WriteIdCommand              = 0x0d,
    SeekCommand                 = 0x0f,
    ScanEqualCommand            = 0x11,
    ScanLowOrEqualCommand       = 0x19,
    ScanHighOrEqualCommand      = 0x1e,
};

static const byte_t BytesInCommand[32] =
{
    1, 1, 9,
    2, // SPECIFY
    2, 9, 9, 2, 1, 9, 2, 1, 9, 6, 1, 3,
    1, 9, 1, 1, 1, 1, 1, 1, 1, 9, 1, 1, 1, 1, 9, 1
};


FloppyController::FloppyController()
    : m_phase(CommandPhase)
    , m_motorsActive(false)
    , m_mainStatusRegister(RQM)
    , m_command(0x00)
{
    for (int i = 0; i < 4; ++i)
    {
        m_drives[i] = 0;
    }
}

void FloppyController::attachDiskDrive(uint number, FloppyDiskDrive* drive)
{
    m_drives[number] = drive;
}

bool FloppyController::in(word_t address, byte_t& value)
{
    bool handled = false;

    // bit 0, 7, 8 and 10
    switch (address & 0x0581)
    {
        // 0xfb7e: read FDC main status register
        case 0x0100:
            value = m_mainStatusRegister;
            break;

        // 0xfb7f: read FDC data register
        case 0x0101:
            break;
    }

    if (handled)
    {
        qDebug() << "[FDC ] IN request at address" << hex << address << "returned value" << hex << value;
    }

    return handled;
}

bool FloppyController::out(word_t address, byte_t value)
{
    bool handled = false;

    // bit 0, 7, 8 and 10
    switch (address & 0x0581)
    {
        // 0xfa7e: floppy motor control
        case 0x0000:
            // bit 0: state of FDD motor
            //            0 = inactive, 1 = active
            m_motorsActive = ((value & 0x01) == 1);
            handled = false;
            break;

        // 0xfb7f: write FDC data register
        case 0x0101:
            writeDataRegister(value);
            handled = true;
            break;
    }

    if (handled)
    {
        qDebug() << "[FDC ] OUT request at address" << hex << address << "with value" << hex << value;
    }

    return handled;
}

void FloppyController::writeDataRegister(byte_t value)
{
    if (m_phase == CommandPhase)
    {
        // new command?
        if (m_command == 0x00)
        {
            m_command = value;
        }
        else
        {
            if (m_commandParameters.size() < BytesInCommand[m_command])
            {
                m_commandParameters.append(value);
            }
        }

        // got complete command sequence?
        if (m_commandParameters.size() == BytesInCommand[m_command])
        {
            interpretCommandCode();
        }
    }
}

void FloppyController::interpretCommandCode()
{
    switch (m_command)
    {
        case SpecifyCommand:
            transitToCommandPhase();
            break;
    }
}

void FloppyController::transitToCommandPhase()
{
    qDebug() << "[FDC ] ==> COMMAND PHASE";

    // reset bit DB4 (CB = FDC Busy)
    m_mainStatusRegister &= ~BUSY;

    // reset bit DB5 (EXM = Execution Mode)
    m_mainStatusRegister &= ~EXM;

    // reset bit DB6 (DIO = Data Input / Output)
    m_mainStatusRegister &= ~DIO;

    // remove previous command sequence
    m_command = 0x00;
    m_commandParameters.clear();

    m_phase = CommandPhase;
}

void FloppyController::transitToExecutionPhase()
{
    qDebug() << "[FDC ] ==> EXECUTION PHASE";

    // transit to execution phase
    m_phase = ExecutionPhase;
    m_mainStatusRegister |= EXM;

    executeCommand();
}

void FloppyController::transitToResultPhase()
{
    qDebug() << "[FDC ] ==> RESULT PHASE";

}

void FloppyController::executeCommand()
{
}
