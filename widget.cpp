#include "widget.h"
#include "./ui_widget.h"
#include "diagram.h"
#include "ui_diagram.h"


#include <QDebug>
#include <QScrollArea>



Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    this->setGeometry(0,0, QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    this->state = States::Off;
    //this->waitingForClick = false;
    //ui->setupUi(this);
    qDebug() << "Main Widget Geometry" <<this->geometry();
    QBoxLayout* mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    //
       MTM_3110 = new Diagram(this, "Calculo 1");
      FSC_5101 = new Diagram(this, "Física 1");
    //QPoint FSC_5101_pos = QPoint()
    //
    FSC_5002 = new Diagram(this, "Física 2");
    MTM_3120 = new Diagram(this,"Calculo 2");
    QWidget *container = new QWidget;
     mainLayout->addWidget(container);
    QGridLayout* containerLayout = new QGridLayout;
    container->setLayout(containerLayout);
    //QScrollArea *scrollArea = new QScrollArea;
    // Set the container as the widget for the scroll area
    //scrollArea->setWidget(container);
    //scrollArea->setWidgetResizable(true);
    //containerLayout->addWidget(scrollArea);

    //primeiro semestre (coluna 0)
    containerLayout->addWidget(MTM_3110, 0,0);
    containerLayout->addWidget(FSC_5101,1,0);
    //segundo semestre (coluna 1)
    containerLayout->addWidget(FSC_5002,0,1);
    containerLayout->addWidget(MTM_3120,1,1);
      // QVector<Diagram*>* FSC_5002_reqs = new QVector<Diagram*>();
      // FSC_5002_reqs->append(FSC_5101);
       // FSC_5002->setPrerequisites(FSC_5002_reqs);

    diagrams.append(MTM_3110);
    diagrams.append(MTM_3120);
    //diagrams.append(FSC_5101);
    //diagrams.append(FSC_5002);


}
void Widget::showEvent(QShowEvent *event){
      // Your custom code here

     // qDebug() << "Widget is about to be shown!";
    //  QPoint globalPos1 = FSC_5101->pos();//FSC_5101->mapToGlobal(QPoint(0, 0));
   //  qDebug() << "FSC_5101 position: " << globalPos1;

      //fsc5101_to_fsc5002->addPoints(*new QList<QPoint> {FSC_5101->mapToGlobal(QPoint(0, 0)),FSC_5002->mapToGlobal(QPoint(0, 0))});
    // QPoint globalPos2 = FSC_5002->pos();//FSC_5002->mapToGlobal(QPoint(0, 0));
     //qDebug() << "FSC_5002 position: " << globalPos2;
     // fsc5101_to_fsc5002->addPoints(*new QList<QPoint> {globalPos1, globalPos2});

      // Call the base class implementation
      QWidget::showEvent(event);
  }
Widget::~Widget()
{
    delete ui;
    delete fsc5101_to_fsc5002;
    delete FSC_5101;
    delete FSC_5002;
    delete MTM_3110;
    qDeleteAll(diagrams);
}

States Widget::getState()
{
    return this->state;
}

void Widget::setCurrentDiagram(Diagram *diagram)
{
    this->currentDiagram = diagram;
}
//CONTINUE FROM HERE!!!!!!!!!!!!!!!!!!
void Widget::saveLines()
{
    //for all diagrams
    //save its name and all its lines to a file
  //  for(Diagram* diagram : diagrams)
   // {
     //   for(int i=0; i<diagram->lines.size(); i++)
       // {

       // }
    //}
        QJsonObject mainObject;
        QJsonObject diagramsObject;

          for(Diagram* diagram : diagrams)
          {
              QJsonArray lines;
              for(int i=0; i<diagram->lines.size(); i++)
              {
                  if(diagram->lines[i]->getPoints().size() != 0)
                  {
                      QJsonArray lineArray;
                      for(QPoint point : diagram->lines[i]->getPoints())
                      {
                          QJsonObject pointObject;
                          qDebug()<<" Saving X:" << point.x();
                          qDebug()<<" Saving Y:" << point.y();
                          pointObject.insert("x", point.x());
                          pointObject.insert("y", point.y());
                          lineArray.push_back(pointObject);
                      }
                      lines.push_back(lineArray);
                  }
                diagramsObject.insert(diagram->name,lines);
              }
          }



        //lineArray.push_back(point);
       //insert single datas first


       mainObject.insert("Diagramas", diagramsObject);




       // lastly we created a JSON document and set mainObject as object of document
       QJsonDocument jsonDoc;
       jsonDoc.setObject(mainObject);

       // Write our jsondocument as json with JSON format
       //ui->txtJsonEncoded->setPlainText( jsonDoc.toJson() );
       this->writeFile("/home/mago/QtProjects/Diagramas2/test.txt", jsonDoc.toJson());


}

void Widget::loadLines()
{
    QByteArray jsonData = this->readFile("/home/mago/QtProjects/Diagramas2/test.txt");
    qDebug() << "jsonData: "<< jsonData.size();
    QJsonParseError error;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &error);
    if (error.error != QJsonParseError::NoError) {
        // Handle JSON parsing error
        qDebug() << "Error Parsing Json in Widget::loadLines -> " <<error.errorString() ;
        return;
    }
    QJsonObject mainObject = jsonDoc.object();
    QJsonObject diagramasObject = mainObject.value("Diagramas").toObject();


    for(Diagram* diagram : diagrams)
    {
        QJsonArray arr_allLines = diagramasObject.value(diagram->name).toArray();
    qDebug()<<"Diagram: "<< diagram->name;
        for(int i=0; i<arr_allLines.size(); i++)
        {
            QJsonArray arr_singleLine =  arr_allLines.at(i).toArray();
            for(int j=0; j<arr_singleLine.size(); j++)
            {
                QJsonObject pointObject = arr_singleLine.at(j).toObject();
                qDebug()<<"loaded X [ij] " << pointObject.value("x").toInt();
                qDebug()<<"loaded y [ij] " << pointObject.value("y").toInt() <<"[" <<i<<j << "]";
                QPoint point = QPoint(pointObject.value("x").toInt(), pointObject.value("y").toInt());
                diagram->lines[i]->addPoint(point);
            }

        }
    }

}
void Widget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "State: " << this->state;
    if (event->button() == Qt::LeftButton) {
     QPoint mousePos = event->pos();
    switch(this->state)
    {
    case WaitingFor:
        break;
    case AddingPoints:
    this->currentDiagram->addPointToLine(mousePos);
        break;
    default:

        qDebug() << "MousePos: "<< mousePos;
    break;
    }
    }
}


void Widget::checkPrerequisitesEvent()
{
     qInfo("checking prerequisetes in widget.cpp");
    for(Diagram* diagram: diagrams)
    {
        if(diagram->isOpen())
        {
            diagram->setStyleSheet("background-color: red;");
        }
        else
        {
          diagram->setStyleSheet("background-color: yellow;");
        }
   }

}

void Widget::writeFile(QString fileName, QString content)
{
    QFile file(fileName);
        // Trying to open in WriteOnly and Text mode
        if(!file.open(QFile::WriteOnly |
                      QFile::Text))
        {
            qDebug() << " Could not open file for writing";
            return;
        }

        // To write text, we use operator<<(),
        // which is overloaded to take
        // a QTextStream on the left
        // and data types (including QString) on the right

        QTextStream out(&file);
        out << content;
        file.flush();
        file.close();
}

QByteArray Widget::readFile(QString fileName)
{
        QFile file(fileName);
        if(!file.open(QFile::ReadOnly |
                      QFile::Text))
        {
            qDebug() << " Could not open the file for reading";
            return nullptr;
        }

        //QTextStream in(&file);
       // QString myText = in.readAll();
        //qDebug() << myText;
        QByteArray jsonData = file.readAll();
        file.close();
        return jsonData;
}

void Widget::paintEvent(QPaintEvent *)
{
    //qDebug() << "MainWidget Position: " << this->mapToGlobal(QPoint(0,0));

    QPainter painter(this);
   // painter.drawLine(QLineF(200,200,400,400));
}
void Widget::resizeEvent(QResizeEvent *event)
    {
    qDebug() <<"Resizing...";
        // Call the base class implementation
        QWidget::resizeEvent(event);
        int yOffSet = 150;
        int xOffSet = 150;
    //    FSC_5101_pos = FSC_5101->mapToParent(QPoint(0,0));//mainWindow_pos+ FSC_5101->mapToGlobal(QPoint(FSC_5101->width()/2, FSC_5101->height()/2));
       //qDebug() << "FSC_5101 position: " << FSC_5101_pos;
  //      FSC_5002_pos =FSC_5002->mapToParent(QPoint(0,0)); //FSC_5002->mapToGlobal(QPoint(FSC_5002->width()/2, FSC_5002->height())/2);
       //qDebug() << "FSC_5002 position: " << FSC_5002_pos;
       //fsc5101_to_fsc5002->setPoints(QList<QPoint>{FSC_5101_pos + QPoint(FSC_5101->width(),yOffSet),QPoint(FSC_5002_pos.x()+0,FSC_5101_pos.y() + 0),FSC_5002_pos+ QPoint(0,FSC_5002->height())});
       //função que atualiza as coordenadas da linha quando resiza a tela


}

void Widget::keyPressEvent(QKeyEvent *event)
{
    switch(event->key())
    {
        case Qt::Key_Escape:
            qDebug() << "Key_Escape";
        break;
        case Qt::Key_Shift:
            qDebug() << "Key_Shift";
            if(this->state == States::Off)
            {
                setState(States::WaitingFor);
            }
            else if(this->state == States::AddingPoints)
            {
                setState(States::Off);

            }
        break;
        case Qt::Key_Plus:
            qDebug() << "Plus";
            if(currentDiagram != nullptr)
            {
                qDebug() << "Plus2";
               // currentDiagram->addNewLine();
               currentDiagram->incIndex();
            }
        break;
        case Qt::Key_1:
            qDebug() << "Saving Lines";
            saveLines();
        break;
        case Qt::Key_2:
            qDebug() << "Loading Lines";
            loadLines();
        break;
    }
    //event->key();
    // Call the base class implementation
    QWidget::keyPressEvent(event);
}
void Widget::setState(States state)
{
    this->state = state;
}


