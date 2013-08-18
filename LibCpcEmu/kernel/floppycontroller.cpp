#include "floppycontroller.h"

#include <QDebug>

#include "exceptions.h"
#include "diskimagefile.h"
#include "floppydiskdrive.h"


// main status register
enum
{
    BUSY = 0x10,    // b4: FDC Busy
    EXM = 0x20,     // b5: Execution Mode
    DIO = 0x40,     // b6: Data Input/Output
    RQM = 0x80      // b7: Request for Master
};

// status register 0
enum
{
    NotReady = 0x08,            // b3
    EquipmentCheck = 0x10,      // b4
    SeekEnd = 0x20,             // b5
    AbnormalTermination = 0x40, // b7 = 0, b6 = 1
    InvalidCommand = 0x80       // b7 = 1, b6 = 0
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
    ScanHighOrEqualCommand      = 0x1e
};

static const int BytesInCommand[32] =
{
    -1,
    -1,
    8, // READ DIAGNOSTIC
    2, // SPECIFY
    1, // SENSE DRIVE STATUS
    8, // WRITE DATA
    8, // READ DATA
    1, // RECALIBRATE
    0, // SENSE INTERRUPT STATUS
    8, // WRITE DELETED DATA
    1, // READ ID
    -1,
    8, // READ DELETED DATA
    5, // WRITE ID
    -1,
    2, // SEEK
    -1,
    8, // SCAN EQUAL
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    -1,
    8, // SCAN LOW OR EQUAL
    -1,
    -1,
    -1,
    -1,
    8, // SCAN HIGH OR EQUAL
    -1
};


FloppyController::FloppyController()
    : m_phase(CommandPhase)
    , m_motorsActive(false)
    , m_mainStatusRegister(RQM)
    , m_st0(0)
    , m_st1(0)
    , m_st2(0)
    , m_selectedDrive(0)
{
    m_command.value = 0x00;

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
            handled = true;
            break;

        // 0xfb7f: read FDC data register
        case 0x0101:
            value = readDataRegister();
            handled = true;
            break;
    }

//    if (handled)
//    {
//        qDebug() << "[FDC ] IN request at address" << hex << address << "returned value" << hex << value;
//    }

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
            handled = true;
            break;

        // 0xfb7f: write FDC data register
        case 0x0101:
            writeDataRegister(value);
            handled = true;
            break;
    }

//    if (handled)
//    {
//        qDebug() << "[FDC ] OUT request at address" << hex << address << "with value" << hex << value;
//    }

    return handled;
}

byte_t FloppyController::readDataRegister()
{
    byte_t data = 0xff;

    switch (m_phase)
    {
        case ResultPhase:
            data = m_resultStack.pop();

            // returned all data?
            if (m_resultStack.isEmpty())
            {
                transitToCommandPhase();
            }
            break;

        case ExecutionPhase:
            data = m_selectedDrive->readData(m_iterator.start + m_iterator.pointer);
            m_iterator.pointer++;

            if (m_iterator.atEnd())
            {
                m_resultStack.push(m_commandParameters[4]);
                m_resultStack.push(m_commandParameters[3]);
                m_resultStack.push(m_selectedDrive->side());
                m_resultStack.push(m_selectedDrive->track());
                m_resultStack.push(m_st2);
                m_resultStack.push(m_st1);
                m_resultStack.push(m_st0);
                transitToResultPhase();
            }
            break;
    }

    return data;
}

void FloppyController::writeDataRegister(byte_t value)
{
    if (m_phase == CommandPhase)
    {
        // new command?
        if (m_command.value == 0x00)
        {
            m_command.value = value;
            // TODO: invalid opcode => ST0 = 0x80 && RESULT PHASE
            Q_ASSERT_X(BytesInCommand[m_command.code] != -1, "FloppyController::writeDataRegister()", "Unknown FDC command: bytes in cmd == -1");
        }
        else
        {
            if (m_commandParameters.size() < BytesInCommand[m_command.code])
            {
                m_commandParameters.append(value);
            }
        }

        // got complete command sequence?
        if (m_commandParameters.size() == BytesInCommand[m_command.code])
        {
            interpretCommandCode();
        }
    }
}

void FloppyController::selectDrive()
{
    uint drive = m_commandParameters[0] & 0x03;
    uint head = (m_commandParameters[0] & 0x04) >> 2;

    // set Unit Select & Head of status register 0
    m_st0 = drive | (head << 2);

    m_selectedDrive = m_drives[drive];
    m_selectedDrive->setSide(head);
}

void FloppyController::interpretCommandCode()
{
    switch (m_command.code)
    {
        case SpecifyCommand:
            transitToCommandPhase();
            break;
        case ReadDataCommand:
            selectDrive();
            transitToExecutionPhase();
            break;
        case RecalibrateCommand:
            selectDrive();
            transitToExecutionPhase();
            break;
        case SenseInterruptStatusCommand:
            m_resultStack.push(m_selectedDrive->track());
            m_resultStack.push(m_st0);

            // data transfer FDC ==> CPU
            m_mainStatusRegister |= DIO;
            m_mainStatusRegister |= BUSY;

            transitToResultPhase();
            break;
        case ReadIdCommand:
            selectDrive();
            transitToExecutionPhase();
            break;
        case SeekCommand:
            selectDrive();
            transitToExecutionPhase();
            break;
        default:
            qCritical() << "[FDC ] unhandled command" << hex << m_command.code;
            throw NotImplementedException("unhandled fdc command");
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
    m_command.value = 0x00;
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

    // reset bit DB5 (EXM = Execution Mode)
    m_mainStatusRegister &= ~EXM;
    m_phase = ResultPhase;
}

void FloppyController::executeCommand()
{
    switch (m_command.code)
    {
        case ReadDataCommand:
            read();
            break;
        case RecalibrateCommand:
            Q_ASSERT(m_commandParameters.size() == 1);

            // retract head to track 0
            seek(0);
            transitToCommandPhase();
            break;
        case ReadIdCommand:
            Q_ASSERT(m_commandParameters.size() == 1);

            readId();
            transitToResultPhase();
            break;
        case SeekCommand:
            Q_ASSERT(m_commandParameters.size() == 2);

            // position head to specified track
            seek(m_commandParameters[1]);

            transitToCommandPhase();
            break;
    }
}

void FloppyController::read()
{
    // disk in drive and motor on?
    if (m_selectedDrive->hasDisk() && m_motorsActive )
    {
        int beginSectorIdx = m_selectedDrive->findSector(m_commandParameters[3]);
        int endSectorIdx = m_selectedDrive->findSector(m_commandParameters[5]);

        m_iterator.start = beginSectorIdx << 9;
        m_iterator.length = 512 + ((beginSectorIdx - endSectorIdx) << 9);
        m_iterator.pointer = 0;
    }
    else
    {
        // set status register 0
        m_st0 |= NotReady;              // NR (Not Ready)
        m_st0 |= AbnormalTermination;   // IC (Interrupt Code) = Abnormal termination
    }

    // data transfer FDC ==> CPU
    m_mainStatusRegister |= DIO;
    m_mainStatusRegister |= BUSY;
}

void FloppyController::readId()
{
    SectorInfo* sector = 0;

    // disk in drive and motor on?
    if (m_selectedDrive->hasDisk() && m_motorsActive )
    {
        sector = m_selectedDrive->readSectorInfo();
    }
    else
    {
        // set status register 0
        m_st0 |= NotReady;              // NR (Not Ready)
        m_st0 |= AbnormalTermination;   // IC (Interrupt Code) = Abnormal termination
    }

    // data transfer FDC ==> CPU
    m_mainStatusRegister |= DIO;
    m_mainStatusRegister |= BUSY;

    if (sector)
    {
        m_resultStack.push(sector->sectorSize);
        m_resultStack.push(sector->sectorId);
        m_resultStack.push(sector->side);
        m_resultStack.push(sector->track);
    }
    m_resultStack.push(m_st2);
    m_resultStack.push(m_st1);
    m_resultStack.push(m_st0);
}

void FloppyController::seek(uint track)
{
    // disk in drive and motor on?
    if (m_selectedDrive->hasDisk() && m_motorsActive )
    {
        m_selectedDrive->seek(track);

        // set status register 0
        m_st0 |= SeekEnd;  // SE (Seek End)
    }
    else
    {
        // set status register 0
        m_st0 |= NotReady;              // NR (Not Ready)
        m_st0 |= AbnormalTermination;   // IC (Interrupt Code) = Abnormal termination
    }
}
