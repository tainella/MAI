#include "beziercurve.h"
#include <QPainter>
#include <QMouseEvent>
#include <array>
#include <QMatrix4x4>
#include <GLUT/glut.h>

beziercurve::beziercurve(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
    , m_curvePen(Qt::black)
{
    m_curvePen.setWidth(2);
    xRot = yRot = zRot = 0;
    scale = 0.1;
    line_count_start = 4;
    selected_line = 0;
    selected_point = 0;
    qreal diff = 0;
    for(int i = 0; i < line_count_start; i++) {
        QVector3D v1(diff-2,diff,diff - 2);
        QVector3D v2(diff,diff,diff+ 2);
        QVector3D v3(diff + 2,diff,diff + 2);
        QVector3D v4(diff + 4,diff,diff -2);
        QVector<QVector3D> dop;
        dop.push_back(v1);
        dop.push_back(v2);
        dop.push_back(v3);
        dop.push_back(v4);
        m_points.push_back(dop);
        diff += 1;
    }
    update();
}

beziercurve::~beziercurve()
{

}


void beziercurve::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void beziercurve::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation(xRot + dy);
        setYRotation(yRot + dx);
    }
    if (event->buttons() & Qt::RightButton) {
        setXRotation(xRot + dy);
        setZRotation(zRot + dx);
    }
    lastPos = event->pos();
    update();
}


QSize beziercurve::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize beziercurve::sizeHint() const
{
    return QSize(400, 400);
}

// обнуление периода
static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360;
    while (angle > 360)
        angle -= 360;
}

// поворот меша на угол angle, относительно оси oX
void beziercurve::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

// поворот меша на угол angle, относительно оси oY
void beziercurve::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}
void beziercurve::changelinecount1() {
    line_count_start += 1;
    QVector<QVector3D> buf(m_points[m_points.size() - 1]);
    buf[0].setX(buf[0].x() + 1);
    buf[1].setX(buf[1].x() + 1);
    buf[2].setX(buf[2].x() + 1);
    buf[3].setX(buf[3].x() + 1);
    buf[0].setY(buf[0].y() + 1);
    buf[1].setY(buf[1].y() + 1);
    buf[2].setY(buf[2].y() + 1);
    buf[3].setY(buf[3].y() + 1);
    m_points.push_back(buf);
    updateGL();
}

void beziercurve::changelinecount2() {
    if (line_count_start >= 3) {
        QVector<QVector<QVector3D>> buf;
        int i = 0;
        for (; i < selected_line; i++) {
            QVector<QVector3D> buf1;
            for (int j = 0; j < 4; j++) {
                buf1.push_back(m_points[i][j]);
            }
            buf.push_back(buf1);
        }
        i++;
        for (; i < m_points.size(); i++) {
            QVector<QVector3D> buf1;
            for (int j = 0; j < 4; j++) {
                buf1.push_back(m_points[i][j]);
            }
            buf.push_back(buf1);
        }
        m_points.clear();
        m_points = buf;
        line_count_start -= 1;
        selected_line = 0;
    }
    updateGL();
}

void beziercurve::changepoint1() {
    selected_point = (selected_point + 1) % 4;
    updateGL();
}

void beziercurve::changepoint2() {
    selected_line = (selected_line + 1) % line_count_start;
    updateGL();
}

// поворот меша на угол angle, относительно оси oZ
void beziercurve::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void beziercurve::setX(const QString value) {
    m_points[selected_line][selected_point].setX(value.toFloat());
    updateGL();
}
void beziercurve::setY(const QString value) {
    m_points[selected_line][selected_point].setY(value.toFloat());
    updateGL();
}
void beziercurve::setZ(const QString value) {
    m_points[selected_line][selected_point].setZ(value.toFloat());
    updateGL();
}

// задание масштаба
void beziercurve::setScale(int value)
{
    scale = value/100.0;
    emit ScaleChanged(value);
    updateGL();
}

// инициализация OpenGL
void beziercurve::initializeGL()
{
    qglClearColor(Qt::black);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
}

// функция отрисовки
void beziercurve::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(xRot, 1.0, 0.0, 0.0);
    glRotatef(yRot, 0.0, 1.0, 0.0);
    glRotatef(zRot, 0.0, 0.0, 1.0);
    glScaled(scale,scale,scale);
    draw();
}

// параметры окна отрисовки
void beziercurve::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-2, +2, -2, +2, 1.0, 15.0);
#else
    glOrtho(-2, +2, -2, +2, 1.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void beziercurve::draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    all_points.clear();
    for (int i = 0; i < m_points.size(); i++) {
        QVector<QVector3D> buf;
        for (double t = 0; t < 1; t = t + 0.001) {
            double x = (1-t)*(1-t)*(1-t)*m_points[i][0].x() + 3*(1-t)*(1-t)*t*m_points[i][1].x() + 3*(1-t)*t*t*m_points[i][2].x() + t*t*t*m_points[i][3].x();
            double y = (1-t)*(1-t)*(1-t)*m_points[i][0].y() + 3*(1-t)*(1-t)*t*m_points[i][1].y() + 3*(1-t)*t*t*m_points[i][2].y() + t*t*t*m_points[i][3].y();
            double z = (1-t)*(1-t)*(1-t)*m_points[i][0].z() + 3*(1-t)*(1-t)*t*m_points[i][1].z() + 3*(1-t)*t*t*m_points[i][2].z() + t*t*t*m_points[i][3].z();
            glPointSize(2);
              glBegin(GL_POINTS);
               glColor3d(0,0,1);
                glVertex3d(x,y,z);
               glEnd();
            QVector3D xyz(x, y, z);
            buf.push_back(xyz);
        }
        all_points.push_back(buf);
    }
    for (int i = 0; i < m_points.size(); i++) { //опорные точки
        glPointSize(5);
          glBegin(GL_POINTS);
          if (i == selected_line && selected_point== 0) {
              glPointSize(50);
              glColor3d(1,1,0);
          }
          else {
           glColor3d(1,0,0);
          }
            glVertex3d(m_points[i][0].x(),m_points[i][0].y(),m_points[i][0].z());
            if (i == selected_line && selected_point == 1) {
                glPointSize(50);
                glColor3d(1,1,0);
            }
            else {
             glColor3d(0,1,0);
            }
           glVertex3d(m_points[i][1].x(),m_points[i][1].y(),m_points[i][1].z());
           if (i == selected_line && selected_point== 2) {
               glPointSize(50);
               glColor3d(1,1,0);
           }
           else {
            glColor3d(0,1,0);
           }
           glVertex3d(m_points[i][2].x(),m_points[i][2].y(),m_points[i][2].z());
           if (i == selected_line && selected_point== 3) {
               glPointSize(50);
               glColor3d(1,1,0);
           }
           else {
            glColor3d(1,0,0);
           }
           glVertex3d(m_points[i][3].x(),m_points[i][3].y(),m_points[i][3].z());
          glEnd();
    }

    for (int i = 0; i < all_points.size() - 1; i++) { //рисуем продольные линии с шагом des1
        for (int j = 0; j < all_points[i].size(); j += 50) {
        glBegin(GL_LINES);
         glColor3d(1,1,1);
         glVertex3d(all_points[i][j].x(),all_points[i][j].y(),all_points[i][j].z());
         glVertex3d(all_points[i+1][j].x(),all_points[i+1][j].y(),all_points[i+1][j].z());
        glEnd();
        }
    }
    for (int i = 0; i < m_points.size() - 1; i++) {
        glPointSize(5);
        glBegin(GL_LINES);
         glColor3d(1,1,1);
         glVertex3d(m_points[i][0].x(),all_points[i][0].y(),all_points[i][0].z());
         glVertex3d(m_points[i+1][0].x(),all_points[i+1][0].y(),all_points[i+1][0].z());
        glEnd();
        glBegin(GL_LINES);
         glColor3d(1,1,1);
         glVertex3d(m_points[i][3].x(),all_points[i][3].y(),all_points[i][3].z());
         glVertex3d(m_points[i+1][3].x(),all_points[i+1][3].y(),all_points[i+1][3].z());
        glEnd();
    }
}
