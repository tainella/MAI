#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "beziercurve.h"

#include <QMainWindow>
#include <QPushButton>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QPushButton* increase;
    QPushButton* decrease;
    int size;

public slots:
    void sl2();
    void sl1();

private:
    Ui::MainWindow *ui;
    beziercurve* m_editor;
};

#endif // MAINWINDOW_H
