#include "demodialog.h"

DemoDialog::DemoDialog( QWidget* parent, MyGraphicView *myPicture) : QDialog( parent ) {
    this->my = myPicture;
    layout = new QFormLayout;
    m_edit1 = new QLabel(this);
    m_edit1->setText("a:");
    m_edit1->setStyleSheet("color: rgb(255, 255, 255)");
    m_edit1->setObjectName("a:");
    m_edit1->setVisible(true);
    l1 = new QLineEdit(this);
    l1->setVisible(true);
    layout->addRow(m_edit1, l1);
    m_edit2 = new QLabel(this);
    m_edit2->setText("f:");
    m_edit2->setObjectName("f:");
    m_edit2->setStyleSheet("color: rgb(255, 255, 255)");
    l2 = new QLineEdit(this);
    l2->setVisible(true);
    layout->addRow(m_edit2, l2);
    QPushButton* okBtn = new QPushButton( "OK" );
    connect( okBtn, SIGNAL( clicked() ), SLOT( sl1() ) );
    layout->addRow( okBtn );
    this->resize(this->sizeHint());
    setLayout( layout );
}

DemoDialog::~DemoDialog() {
}

QString DemoDialog::getInput() const {
    return m_edit1->text();
}

void DemoDialog::sl1() {
    my->a = this->l1->text().toFloat();
    my->slotAlarmTimer();
    accept();

}
