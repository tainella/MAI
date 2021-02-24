#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "beziercurve.h"

#include <QMainWindow>
#include <QPushButton>
#include <QSlider>
#include <QLineEdit>

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
    beziercurve* widget;
    QSlider *rotXSlider;
    QSlider *rotYSlider;
    QSlider *rotZSlider;
    QSlider *ScaleSlider;
    QLineEdit* changeXline;
    QLineEdit* changeYline;
    QLineEdit* changeZline;
    QPushButton* line1;
    QPushButton* line2;
    QPushButton* point1;
    QPushButton* point2;
};

#endif // MAINWINDOW_H
