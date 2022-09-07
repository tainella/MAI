#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H
#include <QPen>
#include <QBrush>
#include <QWidget>
#include <QtMath>
#include <QGLWidget>
#include <QVector>
#include <QVector3D>
#include <iostream>

class beziercurve : public QGLWidget
{
    Q_OBJECT
public:
    explicit beziercurve(QWidget *parent = 0);
    ~beziercurve();
    void draw();
    void resizeGL(int width, int height);
    void initializeGL();
    void paintGL();
    double scale;
    double toch; //точность аппроксимации
    int line_count_start; //количество линий безье в начале
    int selected_line;
    int selected_point;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *) {

    };

public slots:
    // slots for xyz-rotation slider
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    void setScale(int value);
    void setX(const QString value);
    void setY(const QString value);
    void setZ(const QString value);
    void changelinecount1();
    void changelinecount2();
    void changepoint1();
    void changepoint2();

signals:
    // signaling rotation from mouse movement
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
    void ScaleChanged(int value);
    void clicked();
    void pressed ();
    void valueChanged(float);
    void textEdited(const QString &text);

private:
    const int NUM_POINTS = 4;
    const qreal POINT_RADIUS = 4.0;

    QVector<QVector<QVector3D>> m_points;
    QVector<QVector<QVector3D>> all_points; //буфер для отрисовки всех точек

    QPen        m_curvePen;
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    int xRot;
    int yRot;
    int zRot;
    QPoint lastPos;
};

#endif // BEZIERCURVE_H
