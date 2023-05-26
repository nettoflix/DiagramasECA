#ifndef MYLINE_H
#define MYLINE_H

#include <QFrame>
#include <QVector>



class MyLine
{
public:
    MyLine(QVector<QFrame*> &lines);
    ~MyLine();
    QVector<QFrame*> *lines;
    void setColor(QString str);
};

#endif // MYLINE_H
