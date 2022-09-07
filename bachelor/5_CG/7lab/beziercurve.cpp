#include "beziercurve.h"
#include <QPainter>
#include <QMouseEvent>

beziercurve::beziercurve(QWidget *parent)
    : QWidget(parent)
    , m_curvePen(Qt::black)
    , m_dragging(false)
{
    m_curvePen.setWidth(2);

    m_colors[0] = Qt::yellow;
    m_colors[1] = Qt::red;
    m_colors[2] = Qt::red;
    m_colors[3] = Qt::yellow;

    for (int i = 0; i < NUM_POINTS; i++) {
        m_pens[i]    = QPen(m_colors[i]);
        m_brushes[i] = QBrush(m_colors[i]);
    }
}

beziercurve::~beziercurve()
{

}
void beziercurve::mousePressEvent(QMouseEvent *event)
{
    for(int i = 0; i < NUM_POINTS; i++) {
        if(distance(m_points[i], event->pos()) <= POINT_RADIUS) {
            m_selectedPoint = i;
            m_dragging = true;
            break;
        }
    }
}

void beziercurve::mouseMoveEvent(QMouseEvent *event)
{
    if(m_dragging) {
        m_points[m_selectedPoint] = event->pos();
    }
    update();
}

void beziercurve::mouseReleaseEvent(QMouseEvent *)
{
    m_dragging = false;
}

void beziercurve::resizeEvent(QResizeEvent *)
{
    m_points[StartPoint]    = QPointF(20, height() - 20);
    m_points[ControlPoint1] = QPointF(width() - 20, height() - 20);
    m_points[ControlPoint2] = QPointF(20, 20);
    m_points[EndPoint]      = QPointF(width() - 20, 20);
}

void beziercurve::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing, true);

    painter.setPen(m_curvePen);
    for (qreal t = 0; t < 1; t = t + 0.001) {
        qreal x = (1-t)*(1-t)*(1-t)*m_points[0].x() + 3*(1-t)*(1-t)*t*m_points[1].x() + 3*(1-t)*t*t*m_points[2].x() + t*t*t*m_points[3].x();
        qreal y = (1-t)*(1-t)*(1-t)*m_points[0].y() + 3*(1-t)*(1-t)*t*m_points[1].y() + 3*(1-t)*t*t*m_points[2].y() + t*t*t*m_points[3].y();
        QPointF xy(x, y);
        //painter.setPen(m_pens[0]);
        QBrush* brush = new QBrush(Qt::black);
        painter.setBrush(*brush);
        painter.drawEllipse(xy, POINT_RADIUS, POINT_RADIUS);
    }
    for (int i = 0; i < NUM_POINTS; i++) {
        painter.setPen(m_pens[i]);
        painter.setBrush(m_brushes[i]);
        painter.drawEllipse(m_points[i], POINT_RADIUS, POINT_RADIUS);
    }
}
