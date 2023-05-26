#ifndef CONNECTINGLINE_H
#define CONNECTINGLINE_H

#include <QWidget>
#include <QFrame>
#include <QPainter>
#include <QDebug>
class ConnectingLine : public QWidget
{
    Q_OBJECT
public:
    explicit ConnectingLine(QWidget *parent);
    ~ConnectingLine();
    void setColor(Qt::GlobalColor color);
    void setStartPoint(QPoint start) { m_start = start; }
    void setEndPoint(QPoint end) { m_end = end; }
    void setPoints(QList<QPoint> points);
    QList<QPoint> getPoints();
    void addPoint(QPoint point);
   // void paintLine(QPainter &painter)
   // {
  //  qDebug() << "painting line";

  //          for(int i=0; i<points.size()-1; i++)
   //          {
    //            qDebug() << points[i];
   //              QLine line = QLine(points[i], points[i+1]);
   //              painter.drawLine(line);
   //           }
    //}
protected:
     void paintEvent(QPaintEvent *event);

private:

    QPoint m_start;
    QPoint m_end;
    QList<QPoint>* points;
    Qt::GlobalColor color;

};

#endif // CONNECTINGLINE_H
