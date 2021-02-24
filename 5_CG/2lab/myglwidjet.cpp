#include <QtWidgets>
#include <QtOpenGL>
#include <iostream>
#include <QDebug>

#include "myglwidjet.h"

MyGLWidget::MyGLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    // начальные параметры
    xRot = yRot = zRot = 0;
    modenow = true;
    scale = 0.06;
}

QSize MyGLWidget::minimumSizeHint() const
{
    return QSize(50, 50);
}

QSize MyGLWidget::sizeHint() const
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
void MyGLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

// поворот меша на угол angle, относительно оси oY
void MyGLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

// поворот меша на угол angle, относительно оси oZ
void MyGLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

// задание количества вершин в основании
/*void MyGLWidget::setN(int value)
{
    N = value;
    emit NChanged(value);
    updateGL();
}*/

// задание масштаба
void MyGLWidget::setScale(int value)
{
    scale = value / 100.0;
    emit ScaleChanged(value);
    updateGL();
}

// инициализация OpenGL
void MyGLWidget::initializeGL()
{
    qglClearColor(Qt::black);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    //glEnable(GL_LIGHTING);
    //glEnable(GL_LIGHT0);

    //static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
    //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

// функция отрисовки
void MyGLWidget::paintGL()
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
void MyGLWidget::resizeGL(int width, int height)
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

// нажатие на кнопки мыши
void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

// перемещение мыши
void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
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
}

// отрисовка пирамиды
void MyGLWidget::draw()
{
    glLineWidth(2); // ширина ребер
    //glEnable(GL_CULL_FACE);
    if (modenow == true) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // непрозрачность граней
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    qglColor(Qt::white);
    glBegin(GL_LINE_STRIP);
        glVertex3f(10, -3, 0);
        glVertex3f(-10, -3, 0);
        glVertex3f(-7, 1, 5);
        glVertex3f(7, 1, 5);
        glVertex3f(10, -3, 0);
        glVertex3f(4, 3, 0);
        glVertex3f(-4, 3, 0);
    glEnd();
    glBegin(GL_LINE_STRIP);
        glVertex3f(-4, 3, 0);
        glVertex3f(-10, -3, 0);
    glEnd();
    glBegin(GL_LINE_STRIP);
        glVertex3f(-4, 3, 0);
        glVertex3f(-7, 1, 5);
    glEnd();
    glBegin(GL_LINE_STRIP);
        glVertex3f(4, 3, 0);
        glVertex3f(7, 1, 5);
    glEnd();
    qglColor(Qt::red);
        // основание клина

        glBegin(GL_QUADS);
            glVertex3f(-4, 3, 0);
            glVertex3f(4, 3, 0);
            glVertex3f(10, -3, 0);
            glVertex3f(-10, -3, 0);
        glEnd();
        //боковина клина 1
        qglColor(Qt::red);
        glBegin(GL_QUADS);
            glVertex3f(-7, 1, 5);
            glVertex3f(7, 1, 5);
            glVertex3f(4, 3, 0);
            glVertex3f(-4, 3, 0);
        glEnd();
        qglColor(Qt::red);
        //боковина клина 2
        glBegin(GL_QUADS);
            glVertex3f(-7, 1, 5);
            glVertex3f(-10, -3, 0);
            glVertex3f(10, -3, 0);
            glVertex3f(7, 1, 5);
        glEnd();
        qglColor(Qt::red);
        // треугольник 1
        glBegin(GL_TRIANGLE_STRIP);
            glVertex3f(-10, -3, 0);
            glVertex3f(-7, 1, 5);
            glVertex3f(-4, 3, 0);
            glVertex3f(-10, -3, 0);
        glEnd();
        // треугольник 2
        glBegin(GL_TRIANGLE_STRIP);
            glVertex3f(10, -3, 0);
            glVertex3f(7, 1, 5);
            glVertex3f(4, 3, 0);
            glVertex3f(10, -3, 0);
        glEnd();
}
