#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    increase = new QPushButton("increase");
    connect(increase, SIGNAL(clicked()), SLOT(sl1()));

    decrease = new QPushButton("decrease");
    connect(decrease, SIGNAL(clicked()), SLOT(sl2()));

    QGridLayout *mainLayout = new QGridLayout;
    QWidget *centralWidget = new QWidget(this);
    m_editor = new beziercurve();
        mainLayout->addWidget(m_editor,0,0);
        mainLayout->addWidget(increase,1,0);
        mainLayout->addWidget(decrease,2,0);

        centralWidget->setLayout(mainLayout);
        setCentralWidget(centralWidget);

    size = 2;
}

void MainWindow::sl1() {
    if (size > 1) {
        size -= 1;
    }
}

void MainWindow::sl2() {
    size += 1;
}

MainWindow::~MainWindow()
{
    delete ui;
}

