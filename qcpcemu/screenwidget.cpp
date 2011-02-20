#include "screenwidget.h"

#include <QPainter>
#include <QPaintEvent>


ScreenWidget::ScreenWidget(QWidget *parent)
    : QWidget(parent)
{
}


void ScreenWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)
}
