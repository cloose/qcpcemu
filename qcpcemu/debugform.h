#ifndef DEBUGFORM_H
#define DEBUGFORM_H

#include <QWidget>

namespace Ui {
    class DebugForm;
}


class DebugForm : public QWidget
{
    Q_OBJECT

public:
    explicit DebugForm(QWidget *parent = 0);
    ~DebugForm();

    void update();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DebugForm *ui;
};

#endif // DEBUGFORM_H
