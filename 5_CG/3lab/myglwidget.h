#ifndef MYGLWIDGET_H
#define MYGLWIDGET_H

#include <QGLWidget>

class MyGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit MyGLWidget(QWidget *parent = 0);
    bool modenow;
    double scale;
    double toch;
    void draw();
    void resizeGL(int width, int height);
protected:
    void initializeGL();
    void paintGL();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

public slots:
    // slots for xyz-rotation slider
    void setXRotation(int angle);
    void setYRotation(int angle);
    void setZRotation(int angle);
    //void setN(int value);
    void setScale(int value);

signals:
    // signaling rotation from mouse movement
    void xRotationChanged(int angle);
    void yRotationChanged(int angle);
    void zRotationChanged(int angle);
    //void NChanged(int value);
    void ScaleChanged(int value);
    void clicked();

private:
    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    int xRot;
    int yRot;
    int zRot;
    //int N;
    QPoint lastPos;
};

#endif
