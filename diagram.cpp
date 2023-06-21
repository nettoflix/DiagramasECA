#include "diagram.h"
#include "./ui_diagram.h"
#include <QDebug>
#include <QPalette>
class Widget;
Diagram::Diagram(QWidget *parent, QWidget *mainWidget, QString name) :
    QPushButton(parent),
    ui(new Ui::Diagram)
{
    ui->setupUi(this);
    this->mainWidget = mainWidget;//dynamic_cast<Widget*>(parent);
    this->parent = parent;
    this->name = name;
    this->setStyleSheet("background-color: yellow; margin-top: 50px;");
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->setFixedSize(300,200);
    ui->label->setText(name);
    connect(this, SIGNAL(clicked()), this, SLOT(setActive()));
    connect(this, SIGNAL(checkPrerequisites()), mainWidget, SLOT(checkPrerequisitesEvent()));
    connect(this, SIGNAL(clicked()), this, SLOT(buildLines()));

    for(int i=0; i<8; i++)
   {
     // lines.append(new ConnectingLine(parent));
    }

   lines.append(new ConnectingLine(parent));
    //lines[0]->addPoint(*new QPoint(100,100));
    //lines[0]->addPoint(*new QPoint(300,200));
}

Diagram::~Diagram()
{
    delete ui;
    qDeleteAll(lines);
}

 bool Diagram::isActive() const
{
    return this->active;
}

bool Diagram::isOpen() const
{
    bool open=true;
        qDebug()<< "ISS OPEN?:";
        if(prerequisites != nullptr)
           {
               //QList<Diagram*> keys = prerequisites->value();

               for (Diagram* prerequisite : *prerequisites)
               {
                   if(!prerequisite->isActive())
                   {
                    open=false;
                    //prerequisite->setColor(Qt::blue);
                    qDebug()<< "pre requisite is NOT active!";
                    }
               }
           }
           else
           {
               open = isActive();
               qDebug("PREREQUISITES IS NULLPTR");
           }
            return open;
}

void Diagram::setPrerequisites(QVector<Diagram*> *prerequisites)
{
    this->prerequisites = prerequisites;
}

void Diagram::addPointToLine(QPoint point)
{
    //qDebug() << "Point: " << point;
//qDebug() << "lineIndex: " << lineIndex;
//qDebug() << "lines size: " << lines.size();
lines[lineIndex]->addPoint(point);
}

void Diagram::addNewLine()
{
    //QWidget* parentPtr = this->parent;
    lines.append(new ConnectingLine(parent));
    this->parent->updateGeometry();
}

void Diagram::incIndex()
{
this->lineIndex++;
//if(lineIndex>=lines.size()) lineIndex = 0;
}

bool Diagram::operator==(const Diagram &other) const
{
    return other.objectName() == this->objectName();
}

uint Diagram::myQHash(const Diagram &key)
{
return qHash(key.objectName());
}

void Diagram::setActive()
{
    //qDebug() << this->pos();
    if(this->active)
    {this->active = false;
    this->setStyleSheet("background-color: yellow; margin-top: 50px;");


    }
    else
    {this->active=true;
  this->setStyleSheet("background-color: green; margin-top: 50px;");

    }
    emit checkPrerequisites();
}

void Diagram::buildLines()
{
        Widget* l_parent = dynamic_cast<Widget*>(mainWidget);
        if(l_parent->getState() == States::WaitingFor) //state is waitingFlorClick
        {
            l_parent->setState(States::AddingPoints);
            l_parent->setCurrentDiagram(this);
            //adicionandoPontos = true;
            qDebug() << "BuldingLines";

        }



}

