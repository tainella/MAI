#include <QtGui/QGuiApplication>
#include <QtGui/QKeyEvent>

#include <QtGui/QOpenGLWindow>
#include <QtGui/QOpenGLBuffer>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QOpenGLShaderProgram>
#include <QtGui/QOpenGLVertexArrayObject>
#include <QtMath>
#include <QTimer>


static QString vertexShader =
        "#version 120\n"
        "\n"
        "attribute vec3 position;\n"
        "attribute vec3 color;\n"
        "\n"
        "varying vec3 v_color;\n"
        "uniform mat4 matrixRotation;"
        "\n"
        "void main()\n"
        "{\n"
        "    v_color = color;\n"
        "    gl_Position =  matrixRotation * vec4(position, 1);\n"
        "}\n"
        ;

static QString fragmentShader =
        "#version 120\n"
        "\n"
        "varying vec3 v_color;\n"
        "\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = vec4(v_color, 1);\n"
        "}\n"
        ;
struct Vertex
{
public:
  GLfloat m_position[3],
          m_color[3];
};

struct Window : QOpenGLWindow, QOpenGLFunctions
{
    Window()
    {
        gDegreesRotated = 0;
        xRot = yRot = zRot = 0;
        scale = 0.06;
        //setSurfaceType(OpenGLSurface);
        timer = new QTimer();
        timer->setSingleShot(true);
        connect(timer, SIGNAL(timeout()), this, SLOT(update()));
        timer->start(50);
    }

    Vertex* create(GLfloat x, GLfloat y, GLfloat z, GLfloat x1, GLfloat x2, GLfloat x3) {
        Vertex* dop = new Vertex;
        dop->m_position[0] = x;
        dop->m_position[1] = y;
        dop->m_position[2] = z;
        dop->m_color[0] = x1;
        dop->m_color[1] = x2;
        dop->m_color[2] = x3;
        return dop;
    }
    void createShaderProgram()
    {
        if ( !m_pgm.addShaderFromSourceCode( QOpenGLShader::Vertex, vertexShader)) {
            qDebug() << "Error in vertex shader:" << m_pgm.log();
            exit(1);
        }
        if ( !m_pgm.addShaderFromSourceCode( QOpenGLShader::Fragment, fragmentShader)) {
            qDebug() << "Error in fragment shader:" << m_pgm.log();
            exit(1);
        }
        if ( !m_pgm.link() ) {
            qDebug() << "Error linking shader program:" << m_pgm.log();
            exit(1);
        }
    }

    void createGeometry()
    {
            double toch = 5;
            double toch1 = M_PI / toch;
            double toch2 = M_PI / toch / 2;
            double radius = 1; // центр 0,0,0
            double h = 0;
            double i;
            QOpenGLBuffer* buf;
            QOpenGLVertexArrayObject* vaobuf;
            for(i = 2 * M_PI; i >= toch1; i = i - toch1) {
                buf = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
                vaobuf = new QOpenGLVertexArrayObject(this);
                vaobuf->create();
                vaobuf->bind();
                Vertex c[3] = {*create(0.00, 0.00, 0.0, 1.0, 0.0, 0.0),
                               *create(radius * cos(i),  radius * sin(i), 0.0, 0.0, 1.0, 0.0),
                               *create(radius * cos(i- toch1), radius * sin(i- toch1), 0.0, 0.0, 0.0, 1.0)};
                buf->create();
                buf->setUsagePattern(QOpenGLBuffer::StaticDraw); // StreamDraw
                buf->bind();
                buf->allocate(c, sizeof(c));
                m_pgm.enableAttributeArray("position");
                m_pgm.setAttributeBuffer("position", GL_FLOAT, offsetof(Vertex, m_position), 3, sizeof(Vertex) );
                m_pgm.enableAttributeArray("color");
                m_pgm.setAttributeBuffer("color",  GL_FLOAT, offsetof(Vertex, m_color), 3, sizeof(Vertex) );
                m_vbo1.push_back(buf);
                m_vao1.push_back(vaobuf);
                vaobuf->release();
                buf->release();
            }
            for(i = 0; i <= 2 * M_PI-toch1; i = i + toch1) {
                for (h = 0; h <= M_PI_2; h = h + toch2) {
                        if (h+ toch2 <= M_PI_2) { //края
                            buf = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
                            vaobuf = new QOpenGLVertexArrayObject(this);
                            vaobuf->create();
                            vaobuf->bind();
                            Vertex c[4] = {*create(radius * cos(i) * cos(h), radius * sin(i) * cos(h), radius * sin(h), 1.0, 0.0, 0.0),
                                           *create(radius * cos(i+ toch1) * cos(h), radius * sin(i+ toch1) * cos(h), radius * sin(h), 0.0, 1.0, 0.0),
                                           *create(radius * cos(i+ toch1) * cos(h+ toch2), radius * sin(i+ toch1) * cos(h+ toch2), radius * sin(h+ toch2), 0.0, 0.0, 1.0),
                                           *create(radius * cos(i) * cos(h+ toch2), radius * sin(i) * cos(h+ toch2), radius * sin(h+ toch2), 1.0, 0.0, 0.0)};
                            buf->create();
                            buf->setUsagePattern(QOpenGLBuffer::StaticDraw);
                            buf->bind();
                            buf->allocate(c, sizeof(c));
                            m_pgm.enableAttributeArray("position");
                            m_pgm.setAttributeBuffer("position", GL_FLOAT, offsetof(Vertex, m_position), 3, sizeof(Vertex) );
                            m_pgm.enableAttributeArray("color");
                            m_pgm.setAttributeBuffer("color",  GL_FLOAT, offsetof(Vertex, m_color), 3, sizeof(Vertex) );
                            m_vbo3.push_back(buf);
                            m_vao3.push_back(vaobuf);
                            vaobuf->release();
                            buf->release();
                        }
                        else {
                            buf = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
                            vaobuf = new QOpenGLVertexArrayObject(this);
                            vaobuf->create();
                            vaobuf->bind();
                             Vertex c[3] = {*create(0, 0, radius * sin(h+toch2), 1.0, 0.0, 0.0),
                                            *create(radius * cos(i) * cos(h), radius * sin(i) * cos(h), radius * sin(h), 0.0, 1.0, 0.0),
                                            *create(radius * cos(i+ toch1) * cos(h), radius * sin(i+ toch1) * cos(h), radius * sin(h), 0.0, 0.0, 1.0)};
                             buf->create();
                             buf->setUsagePattern(QOpenGLBuffer::StaticDraw);
                             buf->bind();
                             buf->allocate(c, sizeof(c));
                             m_pgm.enableAttributeArray("position");
                             m_pgm.setAttributeBuffer("position", GL_FLOAT, offsetof(Vertex, m_position), 3, sizeof(Vertex) );
                             m_pgm.enableAttributeArray("color");
                             m_pgm.setAttributeBuffer("color",  GL_FLOAT, offsetof(Vertex, m_color), 3, sizeof(Vertex) );
                             m_vbo2.push_back(buf);
                             m_vao2.push_back(vaobuf);
                             vaobuf->release();
                             buf->release();
                        }
                }
      }
    }

    void initializeGL()
    {
        QOpenGLFunctions::initializeOpenGLFunctions();
        createShaderProgram(); m_pgm.bind();
        createGeometry();
    }

    void resizeGL(int w, int h)
    {
        glViewport(0, 0, w, h);
        update();
    }

    void paintGL()
    {
        if (gDegreesRotated < 360) {
                gDegreesRotated = gDegreesRotated + 1;
        }
        else {
            gDegreesRotated = 1;
        }
        glClear(GL_COLOR_BUFFER_BIT);
        QMatrix4x4 rotation;
        rotation.setToIdentity();
        rotation.rotate(gDegreesRotated, 0, 1, 0);
        rotation.rotate(xRot, 1, 0, 0);
        rotation.rotate(zRot, 0, 0, 1);
        m_pgm.bind();
        m_pgm.setUniformValue("matrixRotation", rotation);
        for (int i = 0; i < m_vao1.size(); i++) {
            m_vao1[i]->bind();
            m_vbo1[i]->bind();
                 glDrawArrays(GL_TRIANGLES, 0, 3);
            m_vao1[i]->release();
            m_vbo1[i]->release();
        }
        for (int i = 0; i < m_vao2.size(); i++) {
            m_vao2[i]->bind();
            m_vbo2[i]->bind();
                glDrawArrays(GL_TRIANGLES, 0, 3);
            m_vao2[i]->release();
            m_vbo2[i]->release();
        }
        for (int i = 0; i < m_vao3.size(); i++) {
            m_vao3[i]->bind();
            m_vbo3[i]->bind();
                glDrawArrays(GL_QUADS, 0, 4);
            m_vao3[i]->release();
            m_vbo3[i]->release();
        }
        timer->start(50);
    }

    void keyPressEvent(QKeyEvent * ev)
    {
        switch (ev->key()) {
           case Qt::Key_Escape:
              exit(0);
              break;
           default:
              QWindow::keyPressEvent(ev);
              break;
        }

    }

    void mousePressEvent(QMouseEvent *event)
    {
        lastPos = event->pos();
    }

    // перемещение мыши
    void mouseMoveEvent(QMouseEvent *event)
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
    // обнуление периода
    static void qNormalizeAngle(int &angle)
    {
        while (angle < 0)
            angle += 360;
        while (angle > 360)
            angle -= 360;
    }

    // поворот меша на угол angle, относительно оси oX
    void setXRotation(int angle)
    {
        qNormalizeAngle(angle);
        if (angle != xRot) {
            xRot = angle;
            update();
        }
    }

    // поворот меша на угол angle, относительно оси oY
    void setYRotation(int angle)
    {
        qNormalizeAngle(angle);
        if (angle != yRot) {
            yRot = angle;
            update();
        }
    }

    // поворот меша на угол angle, относительно оси oZ
    void setZRotation(int angle)
    {
        qNormalizeAngle(angle);
        if (angle != zRot) {
            zRot = angle;
            update();
        }
    }

    QOpenGLShaderProgram     m_pgm;
    QOpenGLContext* m_context;
    QVector<QOpenGLBuffer*> m_vbo1;
    QVector<QOpenGLBuffer*> m_vbo2;
    QVector<QOpenGLBuffer*> m_vbo3;
    QVector<QOpenGLVertexArrayObject*> m_vao1;
    QVector<QOpenGLVertexArrayObject*> m_vao2;
    QVector<QOpenGLVertexArrayObject*> m_vao3;
    QTimer* timer;
    GLfloat gDegreesRotated;
    int xRot;
    int yRot;
    int zRot;
    double scale;
    bool flag = true;
    QPoint lastPos;
signals:
    void clicked();
};

int main(int argc, char *argv[])
{
    QGuiApplication a(argc,argv);
    Window w;
    w.setWidth(640); w.setHeight(480);
    w.show();
    return a.exec();
}
