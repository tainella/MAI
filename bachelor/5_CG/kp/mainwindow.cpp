#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>

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
    slider->setRange(10, 40);
    slider->setSingleStep(1);
    slider->setPageStep(1);
    slider->setTickInterval(1);
    slider->setTickPosition(QSlider::TicksRight);
    return slider;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    widget = new beziercurve();

    rotXSlider = createSlider();
    rotYSlider = createSlider();
    rotZSlider = createSlider();
    ScaleSlider = createSlider1();

    line1 = new QPushButton();
    line1->setText("+line");
    line2 = new QPushButton();
    line2->setText("-line");

    point1 = new QPushButton();
    point1->setText(">point");
    point2 = new QPushButton();
    point2->setText(">line");

    QLabel *echoLabel = new QLabel(tr("change X"));
    changeXline = new QLineEdit;
    changeXline->setPlaceholderText("X coord.");
    changeXline->setFocus();
    changeXline->setFixedSize(widget->width()/10,20);

    QLabel *echoLabel2 = new QLabel(tr("change Y"));
    changeYline = new QLineEdit;
    changeYline->setPlaceholderText("Y coord.");
    changeYline->setFocus();
    changeYline->setFixedSize(widget->width()/10,20);

    QLabel *echoLabel3 = new QLabel(tr("change Z"));
    changeZline = new QLineEdit;
    changeZline->setPlaceholderText("Z coord.");
    changeZline->setFocus();
    changeZline->setFixedSize(widget->width()/10,20);

    connect(widget, SIGNAL(xRotationChanged(int)), rotXSlider, SLOT(setValue(int)));
    connect(rotXSlider, SIGNAL(valueChanged(int)), widget, SLOT(setXRotation(int)));

    connect(widget, SIGNAL(yRotationChanged(int)), rotYSlider, SLOT(setValue(int)));
    connect(rotYSlider, SIGNAL(valueChanged(int)), widget, SLOT(setYRotation(int)));

    connect(widget, SIGNAL(zRotationChanged(int)), rotZSlider, SLOT(setValue(int)));
    connect(rotZSlider, SIGNAL(valueChanged(int)), widget, SLOT(setZRotation(int)));

    connect(widget, SIGNAL(ScaleChanged(int)), ScaleSlider, SLOT(setValue(int)));
    connect(ScaleSlider, SIGNAL(valueChanged(int)), widget, SLOT(setScale(int)));

    connect(changeXline, SIGNAL(textEdited(const QString)), widget, SLOT(setX(const QString)));
    connect(changeYline, SIGNAL(textEdited(const QString)), widget, SLOT(setY(const QString)));
    connect(changeZline, SIGNAL(textEdited(const QString)), widget, SLOT(setZ(const QString)));

    connect(line1, SIGNAL(pressed ()), widget, SLOT(changelinecount1()));
    connect(line2, SIGNAL(pressed ()), widget, SLOT(changelinecount2()));

    connect(point1, SIGNAL(pressed ()), widget, SLOT(changepoint1()));
    connect(point2, SIGNAL(pressed ()), widget, SLOT(changepoint2()));

    QGridLayout *mainLayout = new QGridLayout;

    QWidget *centralWidget = new QWidget(this);

        mainLayout->addWidget(widget,0,0,3,1);
        mainLayout->addWidget(rotXSlider, 0,1);
        mainLayout->addWidget(rotYSlider,0,2);
        mainLayout->addWidget(rotZSlider,0,3);
        mainLayout->addWidget(ScaleSlider,0,4);
        mainLayout->addWidget(echoLabel,1,1);
        mainLayout->addWidget(changeXline,2,1);
        mainLayout->addWidget(echoLabel2,1,2);
        mainLayout->addWidget(changeYline,2,2);
        mainLayout->addWidget(echoLabel3,1, 3);
        mainLayout->addWidget(changeZline,2,3);
        mainLayout->addWidget(line1,3,1);
        mainLayout->addWidget(line2,3,3);
        mainLayout->addWidget(point1,4,2);
        mainLayout->addWidget(point2,5,2);
        mainLayout->setHorizontalSpacing(0);
        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
