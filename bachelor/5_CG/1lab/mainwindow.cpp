#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "mygraphicview.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    myPicture = new MyGraphicView(0);
    setCentralWidget(myPicture);
}

MainWindow::~MainWindow()
{
    delete ui;
}
