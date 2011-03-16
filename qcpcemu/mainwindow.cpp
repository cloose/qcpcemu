#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDockWidget>
#include <QMessageBox>
#include <QTimer>

#include "cpcsystem.h"
#include "floppydiskdrive.h"
#include "keyboard.h"
#include "debugform.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_system(new CpcSystem())
    , m_driveA(new FloppyDiskDrive())
    , m_driveB(new FloppyDiskDrive())
    , m_debugForm(new DebugForm(this))
{
    ui->setupUi(this);

    createActions();
    createDockWindows();

    connect(qApp, SIGNAL(lastWindowClosed()),
            m_system, SLOT(stopSystem()));
    connect(m_debugForm, SIGNAL(setBreakpoint(quint16)),
            this, SLOT(setBreakpoint(quint16)));

    // install event filter for key events
    ui->screenWidget->installEventFilter(m_system->keyboard());

    QTimer::singleShot(0, this, SLOT(delayedInit()));
}

MainWindow::~MainWindow()
{
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

void MainWindow::delayedInit()
{
    m_system->loadExternalRom(7, "amsdos.rom");
    m_system->attachDiskDrive(0, m_driveA);
    m_system->attachDiskDrive(1, m_driveB);
    m_system->setRenderer(ui->screenWidget->renderer());
    ui->screenWidget->setFocus(Qt::OtherFocusReason);

    m_driveA->insertDisk("elitee.dsk");
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

void MainWindow::createActions()
{
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
