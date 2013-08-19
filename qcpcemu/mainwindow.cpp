#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAudioOutput>
#include <QCloseEvent>
#include <QDebug>
#include <QDockWidget>
#include <QFileDialog>
#include <QMessageBox>
#include <QTimer>

#include "cpcsystem.h"
#include "floppydiskdrive.h"
#include "keyboard.h"
#include "debugform.h"
#include "screenwidget.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_screenWidget(new ScreenWidget(this))
    , m_system(new CpcSystem(this))
    , m_driveA(new FloppyDiskDrive())
    , m_driveB(new FloppyDiskDrive())
    , m_debugForm(new DebugForm(this))
    , m_audioOutput(0)
{
    ui->setupUi(this);

    setCentralWidget(m_screenWidget);

    createActions();
    createDockWindows();

    connect(m_debugForm, SIGNAL(setBreakpoint(quint16)),
            this, SLOT(setBreakpoint(quint16)));

    // install event filter for key events
    m_screenWidget->installEventFilter(m_system->keyboard());

    QTimer::singleShot(0, this, SLOT(delayedInit()));
}

MainWindow::~MainWindow()
{
    delete m_driveB;
    delete m_driveA;
    delete m_system;
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    m_system->stop();
    event->accept();
}

void MainWindow::delayedInit()
{
    m_system->loadExternalRom(7, "amsdos.rom");
    m_system->attachDiskDrive(0, m_driveA);
    m_system->attachDiskDrive(1, m_driveB);
    m_system->setRenderer(m_screenWidget->renderer());
    m_screenWidget->setFocus(Qt::OtherFocusReason);

    QAudioFormat format;
    format.setSampleRate(44100);
    format.setChannelCount(2);
    format.setSampleSize(8);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    m_audioOutput = new QAudioOutput(format, this);
    QIODevice* device = m_audioOutput->start();

    m_system->setAudioDevice(device);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(fps()));
    timer->start(1000);

    debugRun();
}

void MainWindow::debugRun()
{
    try
    {
        m_system->run();
        m_debugForm->update();
    }
    catch (std::exception& ex)
    {
        QMessageBox::critical(this, "", ex.what());
    }
}

void MainWindow::debugStep()
{
    m_system->step();
    m_debugForm->update();
}

void MainWindow::setBreakpoint(quint16 address)
{
    m_system->addBreakpoint(address);
}

void MainWindow::insertDiscToDriveA()
{
    // ask user for name of the disk image file
    QString fileName = QFileDialog::getOpenFileName(this, tr("Insert Disc into Drive A:"),
                                                    QString(),
                                                    tr("CPC Disk Image File (*.dsk)"));
    if( !fileName.isEmpty() )
    {
        m_driveA->insertDisk(fileName);
        statusBar()->showMessage(tr("Inserted disc '%1' into drive A:").arg(fileName));
    }
}

void MainWindow::ejectDiscInDriveA()
{
    m_driveA->ejectDisk();
    statusBar()->showMessage(tr("Ejected disc in drive A:"));
}

void MainWindow::insertDiscToDriveB()
{
    // ask user for name of the disk image file
    QString fileName = QFileDialog::getOpenFileName(this, tr("Insert Disc into Drive B:"),
                                                    QString(),
                                                    tr("CPC Disk Image File (*.dsk)"));
    if( !fileName.isEmpty() )
    {
        m_driveB->insertDisk(fileName);
        statusBar()->showMessage(tr("Inserted disc '%1' into drive B:").arg(fileName));
    }
}

void MainWindow::ejectDiscInDriveB()
{
    m_driveB->ejectDisk();
    statusBar()->showMessage(tr("Ejected disc in drive B:"));
}

void MainWindow::resetEmulation()
{
    m_system->reset();
}

#include "qimagescreenrenderer.h"
void MainWindow::fps()
{
    QImageScreenRenderer *r = (QImageScreenRenderer*)m_screenWidget->renderer();

    long frameCount = r->frameCounter();

    qDebug() << "Frames per second" << (frameCount - m_lastFrameCount);

    m_lastFrameCount = frameCount;
}

void MainWindow::createActions()
{
    // file menu
    connect(ui->actInsertDiscA, SIGNAL(triggered()),
            this, SLOT(insertDiscToDriveA()));
    connect(ui->actEjectDiscA, SIGNAL(triggered()),
            this, SLOT(ejectDiscInDriveA()));

    connect(ui->actInsertDiscB, SIGNAL(triggered()),
            this, SLOT(insertDiscToDriveB()));
    connect(ui->actEjectDiscB, SIGNAL(triggered()),
            this, SLOT(ejectDiscInDriveB()));

    // emulation menu
    connect(ui->actResetEmulation, SIGNAL(triggered()),
            this, SLOT(resetEmulation()));

    m_debugRunAction = new QAction(tr("Debug run"), this);
    connect(m_debugRunAction, SIGNAL(triggered()), this, SLOT(debugRun()));
    ui->mainToolBar->addAction(m_debugRunAction);

    m_debugStepAction = new QAction(tr("Debug step"), this);
    connect(m_debugStepAction, SIGNAL(triggered()), this, SLOT(debugStep()));
    ui->mainToolBar->addAction(m_debugStepAction);
}

void MainWindow::createDockWindows()
{
    QDockWidget* dock = new QDockWidget(tr("Debug"), this);
    addDockWidget(Qt::RightDockWidgetArea, dock);
    dock->setWidget(m_debugForm);
}
