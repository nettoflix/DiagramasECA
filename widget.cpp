#include "widget.h"
#include "./ui_widget.h"
#include "diagram.h"
#include "ui_diagram.h"


#include <QDebug>




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

    QWidget *container = new QWidget;

    //container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    container->setGeometry(0,0, 800,800);
    //mainLayout->addWidget(container);

    QGridLayout* gridLayout = new QGridLayout;
    container->setLayout(gridLayout);

    QScrollArea* scrollArea = new QScrollArea;
    scrollArea->setWidget(container);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    //PRIMEIRA FASE
    MTM_3110 = new Diagram(container, this, "Calculo 1");
    FSC_5101 = new Diagram(container, this, "Física 1");
    DAS_5334 = new Diagram(container, this, "Introdução à informática para Automação");
    DAS_5411= new Diagram(container, this, "Introdução à Engenheria de Controle e Automação");
    ECZ_5102 = new Diagram(container, this, "Conservação dos Recursos Naturais");
    EGR_5606 = new Diagram(container, this, "Desenho Técnico para Automação");
    //SEGUNDA FASE
    FSC_5002 = new Diagram(container, this, "Física 2");
    EEL_5105 = new Diagram(container, this,"Circuitos e Técnicas Digitais");
    DAS_5102 = new Diagram(container, this,"Fundamentos da Estrutura da Informação");
    MTM_3121 = new Diagram(container, this,"Algebra Linear");
    MTM_3120 = new Diagram(container, this,"Calculo 2");
    FSC_5122 = new Diagram(container, this,"Física Experimental");
    //TERCEIRA FASE
    DAS_5332 = new Diagram(container, this,"Arquit. e Prog. de Sistemas Microcontrolados");
    DAS_5210 = new Diagram(container, this,"Introdução ao Controle de Processos");
    MTM_3131 = new Diagram(container, this,"Equações Diferenciais Ordinárias");
    MTM_3103= new Diagram(container, this,"Cálculo 3");
    FSC_5113= new Diagram(container, this,"Física 3");
    ECV_5215= new Diagram(container, this,"Mecânica dos Sólidos 1");
    //QUARTA FASE
    DAS_5307 = new Diagram(container, this,"Sistemas de Automação Discreta");
    DAS_5308 = new Diagram(container, this,"Programação de Sistemas Automatizados");
    DAS_5103 = new Diagram(container, this,"Cálculo Numérico para Controle e Automação");
    DAS_5114 = new Diagram(container, this,"Sinais e Sistemas Lineares");
    EEL_7540 = new Diagram(container, this,"Circuitos Elétricos para Automação");
    INE_5108 = new Diagram(container, this,"Estatística e Probabilidade para Ciências Exatas");
    //QUINTA FASE
    DAS_5203 = new Diagram(container, this,"Modelagem e Controle de Sist. a Eventos Discretos");
    DAS_5312 = new Diagram(container, this,"Metodolodia para Desenv. de Sistemas");
    EMC_5425 = new Diagram(container, this,"Fênenomenos de Transporte");
    DAS_5109 = new Diagram(container, this,"Modelagem e Simulação de Processos");
    EEL_7550 = new Diagram(container, this,"Eletrônica Aplicada");
    EMC_5235 = new Diagram(container, this,"Metrologia Industrial");
    //SEXTA FASE
    CNM_7820 = new Diagram(container, this,"Aspectos Econômicos e Sociais da Automação");
    DAS_5314= new Diagram(container, this,"Redes de Computadores para Automação Industrial");
    EMC_5467 = new Diagram(container, this,"Acionamentos Hidráulicos e Pneum. para Automação");
    DAS_5120 = new Diagram(container, this,"Sistemas de Controle");
    EEL_5193 = new Diagram(container, this,"Máquina e Acionamentos Elétricos para Automação");
    DAS_5151 = new Diagram(container, this,"Instrumentação em controle");
    //SÉTIMA FASE
    EMC_5258 = new Diagram(container, this,"Inrodução à Automação da Manafatura");
    DAS_5104 = new Diagram(container, this,"Projeto Integrador");
    DAS_5142 = new Diagram(container, this,"Sistemas Dinâmicos");
    EMC_5251 = new Diagram(container, this,"Introdução à Robótica Industrial");
    EEL_5354 = new Diagram(container, this,"Eletrotécnica para automação");
    DAS_5310 = new Diagram(container, this,"Avaliação de Des. de Sist. da Automação Discreta");
    //OITAVA FASE
    DAS_5501 = new Diagram(container, this,"Estágio em Controle e Automação");
    DAS_5401 = new Diagram(container, this,"Aspectos de Seg. em Sist. de Controle e Automação");
    OPT_PROF8 = new Diagram(container, this,"Optativas Profissionalizantes");
    EPS_5211= new Diagram(container, this,"Programação Econômica e Financeira");
    //NONA FASE
    OPT_PROF16 = new Diagram(container, this,"Optativas Profissionalizantes(16)");
    OPT_LIVR = new Diagram(container, this,"Optativas Livres");
    //DÉCIMA FASE
    DAS_5511 = new Diagram(container, this,"Projeto de Fim de Curso");


     gridLayout->setHorizontalSpacing(100);

    //PRIMEIRA FASE (coluna 0)
    QWidget* fase1 = new FaseTitle(this, "1º fase");
    gridLayout->addWidget(fase1, 0,0);
    gridLayout->addWidget(MTM_3110, 1,0);
    gridLayout->addWidget(FSC_5101,2,0);
    gridLayout->addWidget(DAS_5334,3,0);
    gridLayout->addWidget(DAS_5411,4,0);
    gridLayout->addWidget(ECZ_5102,5,0);
    gridLayout->addWidget(EGR_5606,6,0);

    //SEGUNDA FASE (coluna 1)
    QWidget* fase2 = new FaseTitle(this, "2º fase");
    gridLayout->addWidget(fase2, 0,1);
    gridLayout->addWidget(FSC_5002,1,1);
    gridLayout->addWidget(MTM_3120,2,1);
    gridLayout->addWidget(EEL_5105,3,1);
    gridLayout->addWidget(DAS_5102,4,1);
    gridLayout->addWidget(MTM_3121,5,1);
    gridLayout->addWidget(FSC_5122,6,1);
    //TERCEIRA FASE (coluna 2)
    QWidget* fase3 = new FaseTitle(this, "3º fase");
    gridLayout->addWidget(fase3,0,2);
    gridLayout->addWidget(DAS_5332,1,2);
    gridLayout->addWidget(DAS_5210,2,2);
    gridLayout->addWidget(MTM_3131,3,2);
    gridLayout->addWidget(MTM_3103,4,2);
    gridLayout->addWidget(FSC_5113,5,2);
    gridLayout->addWidget(ECV_5215,6,2);
    //QUARTA FASE (coluna 3)
    QWidget* fase4 = new FaseTitle(this, "4º fase");
    gridLayout->addWidget(fase4,0,3);
    gridLayout->addWidget(DAS_5307,1,3);
    gridLayout->addWidget(DAS_5308,2,3);
    gridLayout->addWidget(DAS_5103,3,3);
    gridLayout->addWidget(DAS_5114,4,3);
    gridLayout->addWidget(EEL_7540,5,3);
    gridLayout->addWidget(INE_5108,6,3);
    //QUINTA FASE (coluna 4)
    QWidget* fase5 = new FaseTitle(this, "5º fase");
    gridLayout->addWidget(fase5,0,4);
    gridLayout->addWidget(DAS_5203,1,4);
    gridLayout->addWidget(DAS_5312,2,4);
    gridLayout->addWidget(EMC_5425,3,4);
    gridLayout->addWidget(DAS_5109,4,4);
    gridLayout->addWidget(EEL_7550,5,4);
    gridLayout->addWidget(EMC_5235,6,4);
    //SEXTA FASE (coluna 5)
    QWidget* fase6 = new FaseTitle(this, "6º fase");
    gridLayout->addWidget(fase6,0,5);
    gridLayout->addWidget(CNM_7820,1,5);
    gridLayout->addWidget(DAS_5314,2,5);
    gridLayout->addWidget(EMC_5467,3,5);
    gridLayout->addWidget(DAS_5120,4,5);
    gridLayout->addWidget(EEL_5193,5,5);
    gridLayout->addWidget(DAS_5151,6,5);
    //SÉTIMA FASE (coluna 6)
    QWidget* fase7 = new FaseTitle(this, "7º fase");
    gridLayout->addWidget(fase7,0,6);
    gridLayout->addWidget(EMC_5258,1,6);
    gridLayout->addWidget(DAS_5104,2,6);
    gridLayout->addWidget(DAS_5142,3,6);
    gridLayout->addWidget(EMC_5251,4,6);
    gridLayout->addWidget(EEL_5354,5,6);
    gridLayout->addWidget(DAS_5310,6,6);
    //OITAVA FASE (coluna 7)
    QWidget* fase8 = new FaseTitle(this, "8º fase");
    gridLayout->addWidget(fase8,0,7);
    gridLayout->addWidget(DAS_5501,1,7);
    gridLayout->addWidget(DAS_5401,2,7);
    gridLayout->addWidget(OPT_PROF8,3,7);
    gridLayout->addWidget(EPS_5211,4,7);
    //NONA FASE (coluna 8)
    QWidget* fase9 = new FaseTitle(this, "9º fase");
    gridLayout->addWidget(fase9,0,8);
    gridLayout->addWidget(OPT_PROF16,1,8);
    gridLayout->addWidget(OPT_LIVR,2,8);
    //DÉCIMA FASE (coluna 9)
    QWidget* fase10 = new FaseTitle(this, "10º fase");
    gridLayout->addWidget(fase10,0,9);
    gridLayout->addWidget(DAS_5511,1,9);

 //   encheLinguica1->setPrerequisites(new QVector<Diagram*>{MTM_3110, FSC_5101});
  //  encheLinguica2->setPrerequisites(new QVector<Diagram*>{MTM_3120, FSC_5002});
   // qDebug() << encheLinguica1->prerequisites->size();
    verticalScrollBar = scrollArea->verticalScrollBar();
    // Get the horizontal scroll bar
    horizontalScrollBar = scrollArea->horizontalScrollBar();

    diagrams.append(MTM_3110);
    diagrams.append(MTM_3120);
    diagrams.append(FSC_5101);
    diagrams.append(FSC_5002);   
    diagrams.append(DAS_5334);
    diagrams.append(DAS_5411);
    diagrams.append(ECZ_5102);
    diagrams.append(EGR_5606);
    diagrams.append(EEL_5105);
    diagrams.append(DAS_5102);
    diagrams.append(MTM_3121);
    diagrams.append(FSC_5122);
    diagrams.append(DAS_5332);
    diagrams.append(DAS_5210);
    diagrams.append(MTM_3131);
    diagrams.append(MTM_3103);
    diagrams.append(FSC_5113);
    diagrams.append(ECV_5215);

    diagrams.append(DAS_5307);
    diagrams.append(DAS_5308);
    diagrams.append(DAS_5103);
    diagrams.append(DAS_5114);
    diagrams.append(EEL_7540);
    diagrams.append(INE_5108);
    diagrams.append(DAS_5203);
    diagrams.append(DAS_5312);
    diagrams.append(EMC_5425);
    diagrams.append(DAS_5109);
    diagrams.append(EEL_7550);
    diagrams.append(EMC_5235);
    diagrams.append(CNM_7820);
    diagrams.append(DAS_5314);
    diagrams.append(EMC_5467);
    diagrams.append(DAS_5120);
    diagrams.append(EEL_5193);
    diagrams.append(DAS_5151);
    diagrams.append(EMC_5258);
    diagrams.append(DAS_5104);
    diagrams.append(DAS_5142);
    diagrams.append(EMC_5251);
    diagrams.append(EEL_5354);
    diagrams.append(DAS_5310);
    diagrams.append(DAS_5501);
    diagrams.append(DAS_5401);
    diagrams.append(OPT_PROF8);
    diagrams.append(EPS_5211);
    diagrams.append(OPT_PROF16);
    diagrams.append(OPT_LIVR);
    diagrams.append(DAS_5511);



    QRect combinedRect = container->childrenRect();

    // Access the width and height
   containerWidth = combinedRect.width();
    containerHeight = combinedRect.height();
}
void Widget::showEvent(QShowEvent *event){
      // Your custom code here

     qDebug() << "Widget is about to be shown!";
     qDebug() << "containerWidth and height: " << containerWidth << " | " << containerHeight;

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
               // diagram->lines.append(new ConnectingLine(this));
            }

        }
    }

}
void Widget::mousePressEvent(QMouseEvent *event)
{
    qDebug() << "State: " << this->state;
    qDebug() << "H state: " << this->use_oldH_mousePos;
     qDebug() << "V state: " << this->use_oldV_mousePos;
    if (event->button() == Qt::LeftButton) {
     QPoint mousePos = event->pos();

     //if  "H" is pressed, return mouse pos with old horizontal pos
     if(use_oldH_mousePos)
     {
     mousePos = QPoint(mousePos.x(), oldMousePos.y());
     oldMousePos = mousePos;
     }
     else if(use_oldV_mousePos)
     {
     mousePos = QPoint(oldMousePos.x(), mousePos.y());
     oldMousePos = mousePos;
     }
     else
     {
        oldMousePos = mousePos;
     }
     //if "V" is pressed, return mouse pos with old vertical pos



     int verticalScrollPosition = verticalScrollBar->value();
     int horizontalScrollPosition = horizontalScrollBar->value();
     QPoint mousePosMapped(horizontalScrollPosition+ mousePos.x(), verticalScrollPosition+ mousePos.y());
     //qDebug() << "MousePos relatively to screen: "<< mousePos;
     //qDebug() << "MousePos relatively to scroll area: "<< mousePos2;
    switch(this->state)
    {
    case WaitingFor:
        break;
    case AddingPoints:
    this->currentDiagram->addPointToLine(mousePosMapped);
        break;
    default:
        break;
    }
    }
}


void Widget::checkPrerequisitesEvent()
{
     qInfo("checking prerequisetes in widget.cpp");
    for(Diagram* diagram: diagrams)
    {
        if(diagram->isOpen() && !diagram->isActive())
        {
            //qDebug() << diagram->name << " is open!";
            diagram->paintDiagramColor(MyConstants::my_blue);
        }
        else if (!diagram->isActive() | !diagram->isOpen())
        {
            diagram->paintDiagramColor(MyConstants::my_red);
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
//qDebug() << "OI";
// Get the current scroll positions


//qDebug() << verticalScrollPosition;
//qDebug() << horizontalScrollPosition;
 QPainter painter(this);
 //painter.drawLine(QLineF(200,300,400,400));
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
            //    qDebug() << "Plus2";
               //currentDiagram->addNewLine();
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
        case Qt::Key_H:
            use_oldH_mousePos = true;
        break;
        case Qt::Key_V:
            use_oldV_mousePos = true;

        break;
        default:

        break;
    }
    //event->key();
    // Call the base class implementation
    QWidget::keyPressEvent(event);
}

void Widget::keyReleaseEvent(QKeyEvent *event)
{
    switch(event->key())
    {
    break;
    case Qt::Key_H:
        use_oldH_mousePos = false;
    break;
    case Qt::Key_V:
        use_oldV_mousePos = false;
    break;

    }
}
void Widget::setState(States state)
{
    this->state = state;
}


