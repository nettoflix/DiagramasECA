#include "connectingline.h"
#include <QDebug>
ConnectingLine::ConnectingLine(QWidget *parent)
    : QWidget(parent)
{
   // this->name = name;
    this->setGeometry(parent->geometry());
     this->setGeometry(QRect(0,0,QWIDGETSIZE_MAX,QWIDGETSIZE_MAX));
    //qDebug() << "Geometry:: " <<this->geometry();
    this->setAttribute(Qt::WA_TransparentForMouseEvents);
    this->points = new QList<QPoint>();
    color = Qt::red;

}

ConnectingLine::~ConnectingLine()
{
   points->clear();
   delete points;
}
void ConnectingLine::setColor(Qt::GlobalColor color)
{
    qDebug()<< "Line Color is set to " << color;
this->color = color;
}
void ConnectingLine::setPoints(QList<QPoint> points)
{
        if(points != *this->points){
            //qDebug() << "Points is different";
            this->points->clear();
            for(QPoint point : points)
            {
                this->points->append(point);
              //  qDebug() << point;
            }
        }


    //qDebug() << points[0];
    //if(this->points == nullptr) this->points = new QList<QPoint>(points);
        //this->points = points;
}

QList<QPoint> ConnectingLine::getPoints()
{
    return *this->points;
}
void ConnectingLine::addPoint(QPoint point)
{
    this->points->append(point);
    qDebug() << "points size" << points->size();
}

void ConnectingLine::paintEvent(QPaintEvent *event)
{

    // qDebug("PAINTING LINE");
      QPainter painter(this);
       painter.drawLine(QLineF(100,100,1500,700));
        //painter.drawLine(m_start, m_end);
     // qDebug() << "Points size " << points->size();
      //QLine line = QLine(QPoint(410,593),QPoint(1117,396));
      //QLine line = QLine(QPoint(0,0),QPoint(640,164));

      for(int i=0; i<points->size()-1; i++)
        {
       //   qDebug() << "Points position: " << points->at(i);
         // qDebug() << "Points position: " << points->at(i+1);

          QWidget *l_parent = dynamic_cast<QWidget*>(this->parent());
          //  qDebug() << "mapToParent" << l_parent->mapFromParent(QPoint(0,0));
          QPoint point1 = points->at(i);// + l_parent->mapFromParent(QPoint(0,0)) ;
          QPoint point2 = points->at(i+1); //+ l_parent->mapFromParent(QPoint(0,0)) ;
         // QLine line = QLine(points->at(i), points->at(i+1));
          QLine line = QLine(point1,point2);
          QPen pen(color); // set the pen color to red
          painter.setPen(pen); //
          painter.drawLine(line);

       }
update();



}
