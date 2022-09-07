#include "mygraphicview.h"
#include <QtMath>

MyGraphicView::MyGraphicView(QWidget *parent): QGraphicsView(parent)
{
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключим скроллбар по горизонтали
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // Отключим скроллбар повертикали
    this->setAlignment(Qt::AlignCenter); // Делаем привязку содержимого к центру
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding); // Растягиваем содержимое по виджету

    this->setMinimumHeight(100);
    this->setMinimumWidth(100);

    scene = new QGraphicsScene();
    this->setScene(scene);

    group_1 = new QGraphicsItemGroup(); //ось координат
    group_2 = new QGraphicsItemGroup(); //функция

    scene->addItem(group_1);
    scene->addItem(group_2);

    timer = new QTimer();
    timer->setSingleShot(true);
    connect(timer, SIGNAL(timeout()), this, SLOT(slotAlarmTimer()));
     timer->start(50);
}

MyGraphicView::~MyGraphicView() {

}

void MyGraphicView::slotAlarmTimer() {
    this->deleteItemsFromGroup(group_1);
    this->deleteItemsFromGroup(group_2);
    int width = this->width(); // определяем ширину нашего виджета
    int height = this->height();
    /* Устанавливаем размер сцены по размеру виджета
         * Первая координата - это левый верхний угол,
         * а Вторая - это правый нижний угол
         * */
    scene->setSceneRect(0,0,width,height);

    QPen penBlack(Qt::black); // Задаём чёрную кисть
    QPen penRed(Qt::red);   // Задаём красную кисть
    //рисуем черные оси и красный график
    group_1->addToGroup(scene->addLine(width / 2, 20, width / 2, height - 20, penBlack));
    group_1->addToGroup(scene->addLine(20,height / 2, width - 20, height / 2, penBlack));
    group_1->addToGroup(scene->addLine(width / 2, 20, width / 2 - 15, 35, penBlack));
    group_1->addToGroup(scene->addLine(width / 2, 20, width / 2 + 15, 35, penBlack));
    group_1->addToGroup(scene->addLine(width - 20, height / 2, width - 35, height / 2 + 15, penBlack));
    group_1->addToGroup(scene->addLine(width - 20, height / 2, width - 35, height / 2 - 15, penBlack));
    QGraphicsTextItem *textY = new QGraphicsTextItem("Y", 0);
    textY->setPos(width / 2 - 33, 20);
    QGraphicsTextItem *textX = new QGraphicsTextItem("X", 0);
    textX->setPos(width - 30, height / 2 + 15);
    group_1->addToGroup(textY);
    group_1->addToGroup(textX);
    int step = (width - 40) / 20;
    int istep = 0;
    for (int i = width / 2 - step + 13; i < width - 60; i = i + step) {
        QGraphicsTextItem *textS = new QGraphicsTextItem(QString::number(istep), 0);
        istep = istep + step;
        textS->setPos(20 + i, height / 2 -1);
        group_1->addToGroup(textS);
    }
    istep = -step;
    for (int i = width / 2 - step - step; i > step; i = i - step) {
        QGraphicsTextItem *textS = new QGraphicsTextItem(QString::number(istep), 0);
        istep = istep - step;
        textS->setPos(20 + i, height / 2 - 1);
        group_1->addToGroup(textS);
    }
    //step = (height - 20) / 20;
    istep = -step;
    for (int i = height / 2 + step - 13; i < height - step; i = i + step) {
        QGraphicsTextItem *textS = new QGraphicsTextItem(QString::number(istep), 0);
        istep = istep - step;
        textS->setPos(width / 2 + 7, i);
        group_1->addToGroup(textS);
    }
    istep = step;
    for (int i = height / 2 - step - 13; i > step * 2; i = i - step) {
        QGraphicsTextItem *textS = new QGraphicsTextItem(QString::number(istep), 0);
        istep = istep + step;
        textS->setPos(width / 2 + 7, i);
        group_1->addToGroup(textS);
    }
    step = (width - 40) / 20;
    for (int i = width / 2 - 1; i < width - step; i = i + step) {
        group_1->addToGroup(scene->addLine(i,height / 2, i, height / 2 + 6, penBlack));
    }
    for (int i = width / 2 - 1; i > step; i = i - step) {
        group_1->addToGroup(scene->addLine(i,height / 2, i, height / 2 + 6, penBlack));
    }
    //step = (height - 20) / 20;
    for (int i = height / 2 - 1; i < height - step; i = i + step) {
        group_1->addToGroup(scene->addLine(width / 2,i, width / 2 + 6, i, penBlack));
    }
    for (int i = height / 2 - 1; i > step; i = i - step) {
        group_1->addToGroup(scene->addLine(width / 2,i, width / 2 + 6, i, penBlack));
    }

    //сам график
    QPolygonF polygon;
    QPolygonF polygon1;
    for (float f = 0; f <= 2 * M_PI; f = f + 0.000005) {
        polygon << QPointF(width/2  + a * cos(f)* sqrt(cos(2 * f)), height / 2 - a * sin(f)* sqrt(cos(2 * f)));
        polygon1 << QPointF(width/2 - a * sin(f)* sqrt(cos(2 * f)), height / 2 + a * cos(f)* sqrt(cos(2 * f)));
    }
    group_2->addToGroup(scene->addPolygon(polygon,penRed));
    group_2->addToGroup(scene->addPolygon(polygon1,penRed));
}

void MyGraphicView::resizeEvent(QResizeEvent *event) {
    timer->start(50);
    QGraphicsView::resizeEvent(event);
}


void MyGraphicView::deleteItemsFromGroup(QGraphicsItemGroup *group) {
    foreach( QGraphicsItem *item, scene->items(group->boundingRect())) {
        if(item->group() == group ) {
            delete item;
        }
    }
}
