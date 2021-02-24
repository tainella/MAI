#include <QtWidgets>
#include <QGLWidget>
#include <QDebug>
#include <QInputDialog>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "myglwidget.h"

QSlider* createSlider()
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(0, 360 * 16);
    slider->setSingleStep(16);
    slider->setPageStep(15 * 16);
    slider->setTickInterval(15 * 16);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

QSlider* createSlider1()
{
    QSlider *slider = new QSlider(Qt::Vertical);
    slider->setRange(6, 36);
    slider->setSingleStep(1);
    slider->setPageStep(1);
    slider->setTickInterval(1);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    widget = new MyGLWidget();

    rotXSlider = createSlider();
    rotYSlider = createSlider();
    rotZSlider = createSlider();
    ScaleSlider = createSlider1();

    connect(widget, SIGNAL(xRotationChanged(int)), rotXSlider, SLOT(setValue(int)));
    connect(rotXSlider, SIGNAL(valueChanged(int)), widget, SLOT(setXRotation(int)));

    connect(widget, SIGNAL(yRotationChanged(int)), rotYSlider, SLOT(setValue(int)));
    connect(rotYSlider, SIGNAL(valueChanged(int)), widget, SLOT(setYRotation(int)));

    connect(widget, SIGNAL(zRotationChanged(int)), rotZSlider, SLOT(setValue(int)));
    connect(rotZSlider, SIGNAL(valueChanged(int)), widget, SLOT(setZRotation(int)));

    /*connect(widget, SIGNAL(NChanged(int)), NSlider, SLOT(setValue(int)));
    connect(NSlider, SIGNAL(valueChanged(int)), widget, SLOT(setN(int)));
    */

    connect(widget, SIGNAL(ScaleChanged(int)), ScaleSlider, SLOT(setValue(int)));
    connect(ScaleSlider, SIGNAL(valueChanged(int)), widget, SLOT(setScale(int)));
    QGridLayout *mainLayout = new QGridLayout;
    QWidget *centralWidget = new QWidget(this);

    mode = new QPushButton("change mode");
    connect(mode, SIGNAL(clicked()), SLOT(sl1()));

    bool ok;
    widget->toch = QInputDialog::getDouble(this, tr("Input"),
                                           tr("Точность:"), 0.5, -10000, 10000, 2, &ok,
                                           Qt::WindowFlags());
        mainLayout->addWidget(widget);
        mainLayout->addWidget(mode,2,0);
        mainLayout->addWidget(rotXSlider, 0, 1);
        mainLayout->addWidget(rotYSlider,0,2);
        mainLayout->addWidget(rotZSlider,0,3);

        //mainLayout->addWidget(NSlider);
        mainLayout->addWidget(ScaleSlider,0,4);
        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);
}

void MainWindow::sl1() {
    if (widget->modenow == true) {
        widget->modenow = false;
    }
    else {
        widget->modenow = true;
    }
    widget->updateGL();
}

MainWindow::~MainWindow()
{
    delete rotXSlider;
    delete rotYSlider;
    delete rotZSlider;
    delete NSlider;
    delete ScaleSlider;
    delete widget;
}
