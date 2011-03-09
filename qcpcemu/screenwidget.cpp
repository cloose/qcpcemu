#include "screenwidget.h"

#include <QPainter>
#include <QPaintEvent>

#include "qimagescreenrenderer.h"


ScreenWidget::ScreenWidget(QWidget* parent)
    : QWidget(parent)
    , m_renderer(new QImageScreenRenderer(this))
{
}

QSize ScreenWidget::minimumSizeHint() const
{
    return QSize(800, 600);
}

void ScreenWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    QImage buffer = m_renderer->screenBuffer();
    painter.drawImage(0, 0, buffer);
}

ScreenRenderer* ScreenWidget::renderer() const
{
    return m_renderer;
}
