#include "diagram.h"
#include "./ui_diagram.h"
#include <QDebug>
#include <QPalette>
class Widget;
Diagram::Diagram(QWidget *parent, QString name) :
    QPushButton(parent),
    ui(new Ui::Diagram)
{
    ui->setupUi(this);
    this->parent = dynamic_cast<Widget*>(parent);
    this->name = name;

    this->setStyleSheet("background-color: yellow; margin-top: 50px;");
    this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    this->setFixedSize(300,200);
    ui->label->setText(name);

    connect(this, SIGNAL(clicked()), this, SLOT(setActive()));
    connect(this, SIGNAL(checkPrerequisites()), parent, SLOT(checkPrerequisitesEvent()));
    connect(this, SIGNAL(clicked()), this, SLOT(buildLines()));

    for(int i=0; i<8; i++)
    {
        lines.append(new ConnectingLine(parent));
    }

    //lines.append(new ConnectingLine(parent));
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
    qDebug() << "Point: " << point;
qDebug() << "lineIndex: " << lineIndex;
qDebug() << "lines size: " << lines.size();
lines[lineIndex]->addPoint(point);
}

void Diagram::addNewLine()
{
    QWidget* parentPtr = parent;
    lines.append(new ConnectingLine(parentPtr));
}

void Diagram::incIndex()
{
this->lineIndex++;
if(lineIndex>=lines.size()) lineIndex = 0;
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
        if(parent->getState() == States::WaitingFor) //state is waitingFlorClick
        {
            parent->setState(States::AddingPoints);
            parent->setCurrentDiagram(this);
            //adicionandoPontos = true;
            qDebug() << "BuldingLines";

        }



}

