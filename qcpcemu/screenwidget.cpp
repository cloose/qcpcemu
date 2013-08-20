#include "screenwidget.h"

#include <QDebug>
#include <QPainter>
#include <QPaintEvent>

#include "qimagescreenrenderer.h"


ScreenWidget::ScreenWidget(QWidget* parent)
    : QWidget(parent)
    , m_renderer(new QImageScreenRenderer(this))
    , m_scaleFactor(1.0)
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

    QSize screen(640, 400);
    screen *= m_scaleFactor;

    int borderHeight = (height() - screen.height()) / 2;
    int borderWidth = (width() - screen.width()) / 2;

    if (m_renderer)
    {
        QImage buffer = m_renderer->screenBuffer().scaled(screen.width(), screen.height());
        painter.drawImage(QPoint(borderWidth, borderHeight), buffer);
    }
}

void ScreenWidget::resizeEvent(QResizeEvent *event)
{
    QSize screen(800, 600);
    screen.scale(size(), Qt::KeepAspectRatio);

    m_scaleFactor = screen.height() / 600.0;
}

ScreenRenderer* ScreenWidget::renderer() const
{
    return m_renderer;
}
