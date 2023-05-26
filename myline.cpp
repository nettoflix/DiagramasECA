#include "myline.h"
#include <QDebug>
MyLine::MyLine(QVector<QFrame*> &lines)
{
    this->lines = &lines;
    qDebug(qUtf8Printable(this->lines->at(1)->objectName()));
    for(QFrame* line : lines)
    {
        line->setFrameShadow(QFrame::Plain);
        line->setStyleSheet("background-color: blue; border: 1px solid blue;");
        //line->setMinimumWidth(200);
        //line->setMaximumWidth(200);
    }
}

MyLine::~MyLine()
{
delete this->lines;
}

void MyLine::setColor(QString str)
{
    for(QFrame* line : *lines)
    {
        //line->setFrameShadow(QFrame::Plain);
        //line_1->setStyleSheet("background-color: red; border: 5px solid red;");
        //line->setMinimumWidth(200);
        //line->setMaximumWidth(200);
        line->setStyleSheet("background-color: " + str + "; border: 1px solid " + str + ";");
    }
}
