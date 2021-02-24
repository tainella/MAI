#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QWidget>
#include <QSlider>
#include <QMainWindow>
#include <QPushButton>
#include "myglwidget.h"

namespace Ui {
    class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
private:
    Ui::MainWindow *ui;
    MyGLWidget* widget;
    QSlider *rotXSlider;
    QSlider *rotYSlider;
    QSlider *rotZSlider;
    QSlider * NSlider;
    QSlider *ScaleSlider;
    QPushButton* mode;
private slots:
    void sl1();
};
#endif
