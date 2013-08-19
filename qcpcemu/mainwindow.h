#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CpcSystem;
class DebugForm;
class FloppyDiskDrive;
class ScreenWidget;
class QAudioOutput;

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
    void closeEvent(QCloseEvent* event);

private slots:
    void delayedInit();
    void debugRun();
    void debugStep();
    void setBreakpoint(quint16 address);

    // file menu
    void insertDiscToDriveA();
    void ejectDiscInDriveA();
    void insertDiscToDriveB();
    void ejectDiscInDriveB();

    // emulation menu
    void resetEmulation();

    void fps();

private:
    void createActions();
    void createDockWindows();

    Ui::MainWindow *ui;
    ScreenWidget* m_screenWidget;
    CpcSystem* m_system;
    FloppyDiskDrive* m_driveA;
    FloppyDiskDrive* m_driveB;
    DebugForm* m_debugForm;

    QAction* m_debugRunAction;
    QAction* m_debugStepAction;

    QAudioOutput* m_audioOutput;
    long m_lastFrameCount;
};

#endif // MAINWINDOW_H
