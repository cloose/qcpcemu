#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CpcSystem;
class DebugForm;
class FloppyDiskDrive;

namespace Ui {
    class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private slots:
    void delayedInit();
    void debugRun();
    void debugStep();
    void setBreakpoint(quint16 address);

private:
    void createActions();
    void createDockWindows();

    Ui::MainWindow *ui;
    CpcSystem* m_system;
    FloppyDiskDrive* m_driveA;
    FloppyDiskDrive* m_driveB;
    DebugForm* m_debugForm;

    QAction* m_debugRunAction;
    QAction* m_debugStepAction;
};

#endif // MAINWINDOW_H
