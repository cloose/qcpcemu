#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDockWidget>

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
}

MainWindow::~MainWindow()
{
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

void MainWindow::debugStep()
{
    m_system->step();
    m_debugForm->update();
}

void MainWindow::createActions()
{
    m_debugStepAction = new QAction(tr("Debug step"), this);
    connect(m_debugStepAction, SIGNAL(triggered()), this, SLOT(debugStep()));
    ui->mainToolBar->addAction(m_debugStepAction);
}

void MainWindow::createDockWindows()
{
    QDockWidget* dock = new QDockWidget(tr("Debug"), this);
    dock->setWidget(m_debugForm);
    addDockWidget(Qt::RightDockWidgetArea, dock);
}
