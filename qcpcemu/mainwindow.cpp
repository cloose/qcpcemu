#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDockWidget>
#include <QTimer>

#include "cpcsystem.h"
#include "debugform.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_system(new CpcSystem())
    , m_debugForm(new DebugForm(this))
{
    ui->setupUi(this);

    createActions();
    createDockWindows();

    connect(qApp, SIGNAL(lastWindowClosed()),
            m_system, SLOT(stopSystem()));
    connect(m_debugForm, SIGNAL(setBreakpoint(quint16)),
            this, SLOT(setBreakpoint(quint16)));

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
    m_system->setRenderer(ui->screenWidget->renderer());
}

void MainWindow::debugRun()
{
    m_system->run();
    m_debugForm->update();
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
