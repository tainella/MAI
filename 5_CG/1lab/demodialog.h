#ifndef DEMODIALOG_H
#define DEMODIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFormLayout>
#include <QShortcut>
#include "mygraphicview.h"

class DemoDialog : public QDialog
{
    Q_OBJECT
public:
    DemoDialog( QWidget* parent = 0, MyGraphicView *myPicture = 0);
    ~DemoDialog();
    QString getInput() const;

    signals:
        void applied();
        void clicked();
    private slots:
        void sl1();
    private:
        MyGraphicView *my;
        QLabel* m_edit1;
        QLabel* m_edit2;
        QLineEdit* l1;
        QShortcut *keyA;
        QLineEdit* l2;
        QFormLayout *layout;
    };

#endif // DEMODIALOG_H
