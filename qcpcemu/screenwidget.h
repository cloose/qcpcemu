#ifndef SCREENWIDGET_H
#define SCREENWIDGET_H

#include <QtGui\qwidget.h>


class ScreenWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenWidget(QWidget* parent = 0);

protected:
    virtual void paintEvent(QPaintEvent* event);
};

#endif // SCREENWIDGET_H
