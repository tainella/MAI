
QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BezierCurveEditor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    beziercurveeditor.cpp

HEADERS  += mainwindow.h \
    beziercurveeditor.h

FORMS    += mainwindow.ui
