#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H
#include <QPen>
#include <QBrush>
#include <QWidget>
#include <QtMath>

class beziercurve : public QWidget
{
    Q_OBJECT
public:
    explicit beziercurve(QWidget *parent = 0);
    ~beziercurve();

protected:
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);

signals:

public slots:

private:
    enum PointIndices {
        StartPoint = 0,
        ControlPoint1 = 1,
        ControlPoint2 = 2,
        EndPoint = 3
    };

   //Functions
private:
    qreal distance(QPointF a, QPointF b) {
        QPointF diff = a - b;
        return sqrt(diff.x()*diff.x() + diff.y()*diff.y());
    }

private:
    const int NUM_POINTS = 4;
    const qreal POINT_RADIUS = 4.0;

    QPointF         m_points[4];
    QPen            m_pens[4];
    QBrush          m_brushes[4];
    Qt::GlobalColor m_colors[4];

    QPen        m_curvePen;

    bool        m_dragging;
    int         m_selectedPoint;
};

#endif // BEZIERCURVE_H
