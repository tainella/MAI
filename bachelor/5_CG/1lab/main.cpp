#include "mainwindow.h"
#include "demodialog.h"

#include <QApplication>
#include <QShortcut>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow* w = new MainWindow(0);
    w->show();
    DemoDialog* d = new DemoDialog(0,w->myPicture);
    d->show();
    return a.exec();
}
