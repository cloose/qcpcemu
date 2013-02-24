#include "screenwidget.h"

#include <QDebug>
#include <QPainter>
#include <QPaintEvent>

#include "qimagescreenrenderer.h"


ScreenWidget::ScreenWidget(QWidget* parent)
    : QWidget(parent)
    , m_renderer(new QImageScreenRenderer(this))
{
    setAttribute(Qt::WA_OpaquePaintEvent, true);
}

ScreenWidget::~ScreenWidget()
{
    delete m_renderer; m_renderer = 0;
}

QSize ScreenWidget::minimumSizeHint() const
{
    return QSize(800, 600);
}

void ScreenWidget::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    painter.fillRect(event->rect(), m_renderer->borderColor());

    int screenHeight = 200 * 2;
    int screenWidth = 640;

    int borderHeight = (height() - screenHeight) / 2;
    int borderWidth = (width() - screenWidth) / 2;

    if (m_renderer)
    {
        QImage buffer = m_renderer->screenBuffer().scaled(screenWidth, screenHeight);
        painter.drawImage(QPoint(borderWidth, borderHeight), buffer);
    }
}

ScreenRenderer* ScreenWidget::renderer() const
{
    return m_renderer;
}
