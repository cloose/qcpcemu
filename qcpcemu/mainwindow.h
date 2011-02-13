#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class CpcSystem;
class DebugForm;

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
    void debugStep();

private:
    void createActions();
    void createDockWindows();

    Ui::MainWindow *ui;
    CpcSystem* m_system;
    DebugForm* m_debugForm;

    QAction* m_debugStepAction;
};

#endif // MAINWINDOW_H
