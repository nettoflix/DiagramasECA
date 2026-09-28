#include "widget.h"
#include "./ui_widget.h"
#include "diagram.h"
#include "ui_diagram.h"


#include <QDebug>
#include <QApplication>
#include <QHBoxLayout>
#include <QToolButton>
#include <QShortcut>
#include <QIcon>
#include <QPainter>




Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{

    this->setGeometry(0,0, QWIDGETSIZE_MAX,QWIDGETSIZE_MAX);
    this->state = States::Off;
    setWindowTitle(QString::fromUtf8("Fluxograma ECA — currículo 20241 (não oficial, confira no CAGR)"));
    //this->waitingForClick = false;
    //ui->setupUi(this);
    this->defaultSpaceWidth = 10;
    // path = "/home/nettoflix/Netto/Desenvolvimento/DiagramasECA/test.txt";
    path = QCoreApplication::applicationDirPath() + "/files/saved.txt";
    qDebug("path: [%s]", path.toLatin1().data());
    QBoxLayout* mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(4, 4, 4, 4);
    mainLayout->setSpacing(4);
    this->setLayout(mainLayout);

    container = new QWidget;

    container->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    //container->setGeometry(0,0, 800,800);
    //mainLayout->addWidget(container);

    this->gridLayout = new QGridLayout;
    container->setLayout(gridLayout);

    // o container é embutido numa QGraphicsScene (mais abaixo, depois de
    // preenchido) para que o zoom seja uma transformação linear da vista:
    // caixas, textos e linhas escalam juntos e as coordenadas de saved.txt
    // (relativas ao container) continuam válidas.
    scene = new QGraphicsScene(this);
    view = new ZoomView(scene);
    mainLayout->addWidget(view);

    //PRIMEIRA FASE
    MTM_3110 = new Diagram(container, this, "Cálculo 1");
    FSC_5101 = new Diagram(container, this, "Física 1");
    DAS_5334 = new Diagram(container, this, "Introdução à Informática para Automação");
    DAS_5412= new Diagram(container, this, "Introdução à Engenharia de Controle e Automação");
    ECZ_5102 = new Diagram(container, this, "Conservação de Recursos Naturais");
    EGR_5606 = new Diagram(container, this, "Desenho Técnico para Automação");
    //SEGUNDA FASE
    FSC_5002 = new Diagram(container, this, "Física 2");
    EEL_5105 = new Diagram(container, this,"Circuitos e Técnicas Digitais");
    DAS_5102 = new Diagram(container, this,"Fundamentos da Estrutura da Informação");
    MTM_3121 = new Diagram(container, this,"Álgebra Linear");
    MTM_3120 = new Diagram(container, this,"Cálculo 2");
    FSC_5122 = new Diagram(container, this,"Física Experimental");
    //TERCEIRA FASE
    DAS_5332 = new Diagram(container, this,"Arquit. e Prog. de Sist. Microcontrolados");
    DAS_5210 = new Diagram(container, this,"Introdução ao Controle de Processos");
    MTM_3131 = new Diagram(container, this,"Equações Diferenciais Ordinárias");
    MTM_3103= new Diagram(container, this,"Cálculo 3");
    FSC_5113= new Diagram(container, this,"Física 3");
    ECV_5215= new Diagram(container, this,"Mecânica dos Sólidos 1");
    //QUARTA FASE
    DAS_5307 = new Diagram(container, this,"Sistemas de Automação Discreta");
    DAS_5308 = new Diagram(container, this,"Programação de Sist. Automatizados");
    DAS_5103 = new Diagram(container, this,"Cálc. Numérico para Controle e Automação");
    DAS_5214 = new Diagram(container, this,"Sinais e Sistemas Lineares");
    EEL_7540 = new Diagram(container, this,"Circuitos Elétricos para Automação");
    INE_5108 = new Diagram(container, this,"Estatística e Probabilidade");
    //QUINTA FASE
    DAS_5203 = new Diagram(container, this,"Modelagem e Contr. de Sist. a Eventos Discr.");
    DAS_5320 = new Diagram(container, this,"Metodologia para Desenv. de Sistemas");
    EMC_5425 = new Diagram(container, this,"Fenômenos de Transportes");
    DAS_5109 = new Diagram(container, this,"Modelagem e Simulação de Processos");
    EEL_7550 = new Diagram(container, this,"Eletrônica Aplicada");
    EMC_5235 = new Diagram(container, this,"Metrologia Industrial");
    //SEXTA FASE
    CNM_7820 = new Diagram(container, this,"Aspectos Econômicos e Sociais da Automação");
    DAS_5314= new Diagram(container, this,"Redes de Comp. para Automação");
    EMC_5467 = new Diagram(container, this,"Aci. Hidráulic. e Pneum. para Automação");
    DAS_5120 = new Diagram(container, this,"Sistemas de Controle");
    EEL_5193 = new Diagram(container, this,"Máquinas e Acionamentos Elétricos");
    DAS_5151 = new Diagram(container, this,"Instrumentação em Controle");
    EPS_2351 = new Diagram(container, this,"Gerenciamento de Projetos");
    //SÉTIMA FASE
    EMC_5258 = new Diagram(container, this,"Introdução à Automação da Manufatura");
    DAS_5105 = new Diagram(container, this,"Projeto Integrador");
    DAS_5142 = new Diagram(container, this,"Sistemas Dinâmicos");
    EMC_5251 = new Diagram(container, this,"Introdução à Robótica Industrial");
    EEL_5354 = new Diagram(container, this,"Eletrotécnica para Automação");
    DAS_5318 = new Diagram(container, this,"Avaliação de Desemp. de Processos de Sist. Organizacionais");
    //OITAVA FASE
    DAS_5502 = new Diagram(container, this,"Estágio em Controle e Automação");
    DAS_5402 = new Diagram(container, this,"Ética e Aspectos de Seg. em Sist. de Controle e Automação");
    OPT_PROF8 = new Diagram(container, this,"Optativas Profissionalizantes");
    EPS_7076= new Diagram(container, this,"Gestão Econômica e de Investimentos");
    //NONA FASE
    OPT_PROF16 = new Diagram(container, this,"Optativas Profissionalizantes (16)");
    OPT_LIVR = new Diagram(container, this,"Optativas Livres");
    //DÉCIMA FASE
    DAS_5512 = new Diagram(container, this,"Projeto de Fim de Curso");

    //setPrerequisites
    this->initPrerequisites();
    this->initCargaHoraria();
    this->initGrupos();

    gridLayout->setHorizontalSpacing(this->defaultSpaceWidth);
    // dentro do QGraphicsProxyWidget o container vira janela de topo, e o estilo
    // usaria margem 11 (de janela) em vez de 9 (de widget filho): fixa em 9 para
    // manter a geometria com que files/saved.txt foi gerado
    gridLayout->setContentsMargins(9, 9, 9, 9);
    gridLayout->setVerticalSpacing(6);
    //gridLayout->setVerticalSpacing(this->defaultSpaceHeight);
    QWidget *spacer = new QWidget(); //spacer->setFixedSize(250,200);
    //PRIMEIRA FASE (coluna 0)
    QWidget* fase1 = new FaseTitle(this, "1º fase");
    gridLayout->addWidget(fase1, 0,0);
    gridLayout->addWidget(spacer,1,1);
    gridLayout->addWidget(DAS_5334,2,0);
    gridLayout->addWidget(DAS_5412,3,0);
    gridLayout->addWidget(ECZ_5102,4,0);
    gridLayout->addWidget(EGR_5606,5,0);
    gridLayout->addWidget(spacer,6,0);
    gridLayout->addWidget(MTM_3110, 7,0);
    gridLayout->addWidget(FSC_5101,8,0);


    //SEGUNDA FASE (coluna 1)
    QWidget* fase2 = new FaseTitle(this, "2º fase");
    gridLayout->addWidget(fase2, 0,1);
    gridLayout->addWidget(EEL_5105,1,1);
    gridLayout->addWidget(DAS_5102,2,1);
    gridLayout->addWidget(spacer,3,1);
    gridLayout->addWidget(spacer,4,1);
    gridLayout->addWidget(spacer,5,1);
    gridLayout->addWidget(MTM_3121,6,1);
    gridLayout->addWidget(MTM_3120,7,1);
    gridLayout->addWidget(FSC_5002,8,1);
    gridLayout->addWidget(FSC_5122,9,1);
    //TERCEIRA FASE (coluna 2)
    QWidget* fase3 = new FaseTitle(this, "3º fase");
    gridLayout->addWidget(fase3,0,2);
    //gridLayout->addWidget(spacer,1,2);
    gridLayout->addWidget(DAS_5332,1,2);
    gridLayout->addWidget(spacer,2,2);
    gridLayout->addWidget(spacer,3,2);
    gridLayout->addWidget(spacer,4,2);
    gridLayout->addWidget(DAS_5210,5,2);
    gridLayout->addWidget(MTM_3131,6,2);
    gridLayout->addWidget(MTM_3103,7,2);
    gridLayout->addWidget(FSC_5113,8,2);
    gridLayout->addWidget(ECV_5215,9,2);
    //QUARTA FASE (coluna 3)
    QWidget* fase4 = new FaseTitle(this, "4º fase");
    gridLayout->addWidget(fase4,0,3);
    gridLayout->addWidget(DAS_5307,1,3);
    gridLayout->addWidget(spacer,2,3);
    gridLayout->addWidget(DAS_5308,3,3);
    gridLayout->addWidget(DAS_5103,4,3);
    gridLayout->addWidget(spacer,5,3);
    gridLayout->addWidget(spacer,6,3);
    gridLayout->addWidget(DAS_5214,7,3);
    gridLayout->addWidget(EEL_7540,8,3);
    gridLayout->addWidget(spacer,9,3);
    gridLayout->addWidget(INE_5108,10,3);
    //QUINTA FASE (coluna 4)
    QWidget* fase5 = new FaseTitle(this, "5º fase");
    gridLayout->addWidget(fase5,0,4);
    gridLayout->addWidget(spacer,1,4);
    gridLayout->addWidget(DAS_5203,2,4);
    gridLayout->addWidget(DAS_5320,3,4);
    gridLayout->addWidget(spacer,4,4);
    gridLayout->addWidget(EMC_5425,5,4);
    gridLayout->addWidget(DAS_5109,6,4);
    gridLayout->addWidget(EEL_7550,7,4);
    gridLayout->addWidget(spacer,8,4);
    gridLayout->addWidget(EMC_5235,9,4);
    //SEXTA FASE (coluna 5)
    QWidget* fase6 = new FaseTitle(this, "6º fase");
    gridLayout->addWidget(fase6,0,5);
    gridLayout->addWidget(spacer,1,5);
    gridLayout->addWidget(CNM_7820,2,5);
    gridLayout->addWidget(DAS_5314,3,5);
    gridLayout->addWidget(EPS_2351,4,5);
    gridLayout->addWidget(EMC_5467,5,5);
    gridLayout->addWidget(DAS_5120,6,5);
    gridLayout->addWidget(spacer,7,5);
    gridLayout->addWidget(EEL_5193,8,5);
    gridLayout->addWidget(DAS_5151,9,5);
    //SÉTIMA FASE (coluna 6)
    QWidget* fase7 = new FaseTitle(this, "7º fase");
    gridLayout->addWidget(fase7,0,6);
    gridLayout->addWidget(EMC_5258,1,6);
    gridLayout->addWidget(spacer,2,6);
    gridLayout->addWidget(spacer,3,6);
    gridLayout->addWidget(DAS_5105,4,6);
    gridLayout->addWidget(spacer,5,6);
    gridLayout->addWidget(DAS_5142,6,6);
    gridLayout->addWidget(EMC_5251,7,6);
    gridLayout->addWidget(EEL_5354,8,6);
    gridLayout->addWidget(spacer,9,6);
    gridLayout->addWidget(DAS_5318,10,6);
    //OITAVA FASE (coluna 7)
    QWidget* fase8 = new FaseTitle(this, "8º fase");
    gridLayout->addWidget(fase8,0,7);
    gridLayout->addWidget(spacer,1,7);
    gridLayout->addWidget(DAS_5502,2,7);
    gridLayout->addWidget(spacer,3,7);
    gridLayout->addWidget(spacer,4,7);
    gridLayout->addWidget(spacer,5,7);
    gridLayout->addWidget(spacer,6,7);
    gridLayout->addWidget(spacer,7,7);
    gridLayout->addWidget(spacer,8,7);
    gridLayout->addWidget(DAS_5402,9,7);
    gridLayout->addWidget(OPT_PROF8,10,7);
    gridLayout->addWidget(EPS_7076,11,7);
    //NONA FASE (coluna 8)
    QWidget* fase9 = new FaseTitle(this, "9º fase");
    gridLayout->addWidget(fase9,0,8);
    gridLayout->addWidget(spacer,1,8);
    gridLayout->addWidget(spacer,2,8);
    gridLayout->addWidget(spacer,3,8);
    gridLayout->addWidget(spacer,4,8);
    gridLayout->addWidget(spacer,5,8);
    gridLayout->addWidget(spacer,6,8);
    gridLayout->addWidget(spacer,7,8);
    gridLayout->addWidget(OPT_PROF16,8,8);
    gridLayout->addWidget(OPT_LIVR,9,8);
    //DÉCIMA FASE (coluna 9)
    QWidget* fase10 = new FaseTitle(this, "10º fase");
    gridLayout->addWidget(fase10,0,9);
    gridLayout->addWidget(spacer,1,9);
    gridLayout->addWidget(DAS_5512,2,9);


    diagrams.append(MTM_3110);
    diagrams.append(MTM_3120);
    diagrams.append(FSC_5101);
    diagrams.append(FSC_5002);
    diagrams.append(DAS_5334);
    diagrams.append(DAS_5412);
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
    diagrams.append(DAS_5214);
    diagrams.append(EEL_7540);
    diagrams.append(INE_5108);
    diagrams.append(DAS_5203);
    diagrams.append(DAS_5320);
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
    diagrams.append(EPS_2351);
    diagrams.append(EMC_5258);
    diagrams.append(DAS_5105);
    diagrams.append(DAS_5142);
    diagrams.append(EMC_5251);
    diagrams.append(EEL_5354);
    diagrams.append(DAS_5318);
    diagrams.append(DAS_5502);
    diagrams.append(DAS_5402);
    diagrams.append(OPT_PROF8);
    diagrams.append(EPS_7076);
    diagrams.append(OPT_PROF16);
    diagrams.append(OPT_LIVR);
    diagrams.append(DAS_5512);



    // barra de zoom no topo (depois da grade pronta: um botão por fase/coluna)
    mainLayout->insertLayout(0, buildToolbar());

    // tamanho natural da grade (sem esticar), igual ao usado para gerar saved.txt
    container->adjustSize();
    container->installEventFilter(this);
    QGraphicsProxyWidget* proxy = scene->addWidget(container);
    proxy->setPos(0, 0);
    scene->setSceneRect(QRectF(QPointF(0, 0), container->size()));
    for(Diagram* diagram : diagrams)
        for(ConnectingLine* l : diagram->lines)
            l->setGeometry(container->rect());

    // cliques do modo de edição de linhas (Shift) chegam pela viewport
    view->viewport()->installEventFilter(this);

    loadLines();
    checkPrerequisitesEvent();

}
void Widget::showEvent(QShowEvent *event){
    QWidget::showEvent(event);
    if(firstShow)
    {
        firstShow = false;
        view->fitAll(false); // abre mostrando todas as disciplinas
    }
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
    QJsonObject diagramObj;
    for(Diagram* diagram : diagrams)
    {

        QJsonArray lines;
        QJsonObject isActive;
        for(int i=0; i<diagram->lines.size(); i++)
        {
            if(diagram->lines[i]->getPoints().size() != 0)
            {
                QJsonArray pointsArray;
                for(QPoint point : diagram->lines[i]->getPoints())
                {
                    QJsonObject pointObject;
                    // qDebug()<<" Saving X:" << point.x();
                    //qDebug()<<" Saving Y:" << point.y();
                    pointObject.insert("x", point.x());
                    pointObject.insert("y", point.y());
                    pointsArray.push_back(pointObject);
                }
                lines.push_back(pointsArray);
            }
            //  isActive.insert()
            diagramObj.insert("lines", lines);
            diagramObj.insert("isActive", diagram->isActive());
            diagramsObject.insert(diagram->name,diagramObj);
        }
    }

    mainObject.insert("Diagramas", diagramsObject);




    // lastly we created a JSON document and set mainObject as object of document
    QJsonDocument jsonDoc;
    jsonDoc.setObject(mainObject);

    // Write our jsondocument as json with JSON format
    //ui->txtJsonEncoded->setPlainText( jsonDoc.toJson() );
    this->writeFile(path, jsonDoc.toJson());


}

void Widget::loadLines()
{
    QByteArray jsonData = this->readFile(path);
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
        QJsonObject diagramObj = diagramasObject.value(diagram->name).toObject();
        QJsonArray arr_allLines = diagramObj.value("lines").toArray();
        bool isActive = diagramObj.value("isActive").toBool();
        if(isActive) diagram->setActive(true);

        qDebug()<<"Diagram: "<< diagram->name;
        for(int i=0; i<arr_allLines.size(); i++)
        {
            QJsonArray arr_singleLine =  arr_allLines.at(i).toArray();
            for(int j=0; j<arr_singleLine.size(); j++)
            {
                QJsonObject pointObject = arr_singleLine.at(j).toObject();
                QPoint point = QPoint(pointObject.value("x").toInt(), pointObject.value("y").toInt());
                diagram->lines[i]->addPoint(point);
                // diagram->lines.append(new ConnectingLine(this));
            }
        }
        diagram->lineIndex = arr_allLines.size();
    }

}
// Modo de edição de linhas: recebe o clique já em coordenadas de cena,
// que são as mesmas coordenadas do container usadas em saved.txt.
void Widget::addPointAt(QPoint scenePos)
{
    if(this->state != AddingPoints || currentDiagram == nullptr)
        return;
    if(currentDiagram->getCurrentLine()->size() <= 0) //se for o primeiro ponto da linha, coloca o ponto na disciplina
    {
        qDebug()<< currentDiagram->name<< ": "<< this->currentDiagram->lineIndex<<" ";
        QPoint start(currentDiagram->pos().x()+ currentDiagram->width()+2, currentDiagram->pos().y()+ (80));
        currentDiagram->addPointToLine(start);
        oldMousePos = start;
        return;
    }
    //if "H" is pressed, keep the previous vertical position (horizontal segment)
    if(use_oldH_mousePos)
        scenePos = QPoint(scenePos.x(), oldMousePos.y());
    //if "V" is pressed, keep the previous horizontal position (vertical segment)
    else if(use_oldV_mousePos)
        scenePos = QPoint(oldMousePos.x(), scenePos.y());
    oldMousePos = scenePos;
    currentDiagram->addPointToLine(scenePos);
}


void Widget::checkPrerequisitesEvent()
{
    tempCounter+=1;
    // desmarca em cascata quem perdeu um pré-requisito ou a carga horária
    // exigida; repete até estabilizar, pois cada desmarcação reduz as horas
    bool mudou = true;
    while(mudou)
    {
        mudou = false;
        for(Diagram* diagram: diagrams)
        {
            if(diagram->isActive() && !diagram->isOpen())
            {
                diagram->setActive(false);
                mudou = true;
            }
        }
    }

    for(Diagram* diagram: diagrams)
    {
        if(diagram->isActive())
            continue; // verde, pintado por setActive(true)
        diagram->setActive(false); // linhas vermelhas
        diagram->aplicarEstado(diagram->isOpen() ? Diagram::Disponivel : Diagram::Bloqueada);
    }

    if(horasLabel != nullptr)
    {
        int total = 0;
        for(Diagram* d : diagrams)
            if(d->obrigatoria)
                total += d->cargaHoraria;
        horasLabel->setText(QString::fromUtf8("%1 de %2 h/a obrigatórias")
                            .arg(horasObrigatoriasConcluidas()).arg(total));
    }
}

void Widget::writeFile(QString fileName, QString content)
{
    QFile file(fileName);
    // Trying to open in WriteOnly and Text mode
    if(!file.open(QFile::WriteOnly |
                   QFile::Text))
    {
        qDebug() << " Could not open file for writing "<< fileName;
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
        qDebug() << " Could not open the file for reading " << fileName;
        return nullptr;
    }

    //QTextStream in(&file);
    // QString myText = in.readAll();

    QByteArray jsonData = file.readAll();
    file.close();
    return jsonData;
}

void Widget::clearLines()
{
    setState(States::Off);
    for(Diagram* diagram : diagrams)
    {
        diagram->lineIndex = 0;
        for(ConnectingLine* line : diagram->lines)
        {
            line->clearPoints();
        }
        //diagram->lines.clear();
    }

}

void Widget::paintEvent(QPaintEvent *)
{
    //    qDebug("void Widget::paintEvent(QPaintEvent *)");
    //if(scrollArea==nullptr) return;

    // QPainter painter(scrollArea);
    // painter.setPen(Qt::white); // Set the text color
    // QString text = "Hello, Mouse!"; // Text to display
    // painter.drawText(mousePos, text); // Draw text at the mouse position

}
void Widget::resizeEvent(QResizeEvent *event)
{
    qDebug() <<"Resizing...";
    // Call the base class implementation
    QWidget::resizeEvent(event);
    int yOffSet = 150;
    int xOffSet = 150;
    //    FSC_5101_pos = FSC_5101->mapToParent(QPoint(0,0));//mainWindow_pos+ FSC_5101->mapToGlobal(QPoint(FSC_5101->width()/2, FSC_5101->height()/2));

    //      FSC_5002_pos =FSC_5002->mapToParent(QPoint(0,0)); //FSC_5002->mapToGlobal(QPoint(FSC_5002->width()/2, FSC_5002->height())/2);

    //fsc5101_to_fsc5002->setPoints(QList<QPoint>{FSC_5101_pos + QPoint(FSC_5101->width(),yOffSet),QPoint(FSC_5002_pos.x()+0,FSC_5101_pos.y() + 0),FSC_5002_pos+ QPoint(0,FSC_5002->height())});
    //função que atualiza as coordenadas da linha quando resiza a tela


}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if(event->key() != Qt::Key_Shift)
        shiftAlone = false; // Shift usado como modificador de outra tecla
    switch(event->key())
    {
    case Qt::Key_Escape:
        qDebug() << "Key_Escape";
        break;
    case Qt::Key_Shift:
        // o modo de edição alterna ao SOLTAR o Shift (keyReleaseEvent), para
        // que Shift+clique nos botões de fase não entre no modo sem querer
        if(!event->isAutoRepeat())
            shiftAlone = true;
        break;
    case Qt::Key_Plus:

        if(currentDiagram != nullptr)
        {
            qDebug() << "Plus";
            //currentDiagram->addNewLine();
            currentDiagram->incIndex();
        }
        break;
    case Qt::Key_Minus:

        if(currentDiagram != nullptr)
        {
            qDebug() << "Minus";
            //currentDiagram->addNewLine();
            currentDiagram->decIndex();
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
    case Qt::Key_C:
        clearLines();
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
    case Qt::Key_Shift:
        if(!event->isAutoRepeat() && shiftAlone)
        {
            qDebug() << "Key_Shift";
            if(this->state == States::Off)
            {
                setState(States::WaitingFor);
            }
            else if(this->state == States::AddingPoints)
            {
                setState(States::Off);
            }
        }
        shiftAlone = false;
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
void Widget:: initPrerequisites()
{
    // pré-requisitos do currículo 20241 (curriculoECA.PDF)
    //SEGUNDA FASE
    DAS_5102->setPrerequisites(new QVector<Diagram*>{DAS_5334});
    EEL_5105->setPrerequisites(new QVector<Diagram*>{DAS_5334});
    FSC_5002->setPrerequisites(new QVector<Diagram*>{FSC_5101,MTM_3110});
    FSC_5122->setPrerequisites(new QVector<Diagram*>{FSC_5101});
    MTM_3120->setPrerequisites(new QVector<Diagram*>{MTM_3110});
    //TERCEIRA FASE
    DAS_5210->setPrerequisites(new QVector<Diagram*>{DAS_5412,FSC_5101, MTM_3110});
    DAS_5332->setPrerequisites(new QVector<Diagram*>{EEL_5105});
    ECV_5215->setPrerequisites(new QVector<Diagram*>{FSC_5002, MTM_3120});
    FSC_5113->setPrerequisites(new QVector<Diagram*>{FSC_5002});
    MTM_3103->setPrerequisites(new QVector<Diagram*>{MTM_3120});
    MTM_3131->setPrerequisites(new QVector<Diagram*>{MTM_3120,MTM_3121});
    //QUARTA FASE
    DAS_5103->setPrerequisites(new QVector<Diagram*>{DAS_5102,MTM_3110,MTM_3121});
    DAS_5214->setPrerequisites(new QVector<Diagram*>{DAS_5210, MTM_3131});
    DAS_5307->setPrerequisites(new QVector<Diagram*>{DAS_5412, EEL_5105});
    DAS_5308->setPrerequisites(new QVector<Diagram*>{DAS_5102, DAS_5332});
    EEL_7540->setPrerequisites(new QVector<Diagram*>{FSC_5113, MTM_3131});
    INE_5108->setPrerequisites(new QVector<Diagram*>{MTM_3110});
    //QUINTA FASE
    DAS_5109->setPrerequisites(new QVector<Diagram*>{DAS_5214, EEL_7540});
    DAS_5203->setPrerequisites(new QVector<Diagram*>{DAS_5307});
    DAS_5320->setPrerequisites(new QVector<Diagram*>{DAS_5308});
    EEL_7550->setPrerequisites(new QVector<Diagram*>{EEL_7540});
    EMC_5235->setPrerequisites(new QVector<Diagram*>{EEL_7540});
    EMC_5425->setPrerequisites(new QVector<Diagram*>{FSC_5002, MTM_3103});
    //SEXTA FASE
    DAS_5120->setPrerequisites(new QVector<Diagram*>{DAS_5109});
    DAS_5151->setPrerequisites(new QVector<Diagram*>{DAS_5109, EEL_7550, EMC_5235});
    DAS_5314->setPrerequisites(new QVector<Diagram*>{DAS_5307, DAS_5308});
    EEL_5193->setPrerequisites(new QVector<Diagram*>{EEL_7540});
    EMC_5467->setPrerequisites(new QVector<Diagram*>{DAS_5214, DAS_5307, EMC_5425});
    EPS_2351->setPrerequisites(new QVector<Diagram*>{ECZ_5102, INE_5108});
    //SÉTIMA FASE
    DAS_5105->setPrerequisites(new QVector<Diagram*>{DAS_5109, DAS_5203, DAS_5320, EEL_7550, EPS_2351});
    DAS_5142->setPrerequisites(new QVector<Diagram*>{DAS_5120});
    DAS_5318->setPrerequisites(new QVector<Diagram*>{DAS_5203, INE_5108});
    EEL_5354->setPrerequisites(new QVector<Diagram*>{EEL_5193, EEL_7540});
    EMC_5251->setPrerequisites(new QVector<Diagram*>{DAS_5214});
    EMC_5258->setPrerequisites(new QVector<Diagram*>{DAS_5307});
    //OITAVA FASE
    DAS_5502->setPrerequisites(new QVector<Diagram*>{DAS_5105});
    //DÉCIMA FASE
    DAS_5512->setPrerequisites(new QVector<Diagram*>{DAS_5502});
}

void Widget::initCargaHoraria()
{
    // horas-aula (H/A) de cada disciplina; "false" = optativa (não conta
    // para as exigências de carga horária, que são de horas obrigatórias)
    DAS_5334->setCargaHoraria(72);  DAS_5412->setCargaHoraria(72);  ECZ_5102->setCargaHoraria(36);
    EGR_5606->setCargaHoraria(72);  FSC_5101->setCargaHoraria(72);  MTM_3110->setCargaHoraria(72);
    DAS_5102->setCargaHoraria(72);  EEL_5105->setCargaHoraria(90);  FSC_5002->setCargaHoraria(72);
    FSC_5122->setCargaHoraria(54);  MTM_3120->setCargaHoraria(72);  MTM_3121->setCargaHoraria(72);
    DAS_5210->setCargaHoraria(54);  DAS_5332->setCargaHoraria(72);  ECV_5215->setCargaHoraria(90);
    FSC_5113->setCargaHoraria(72);  MTM_3103->setCargaHoraria(72);  MTM_3131->setCargaHoraria(72);
    DAS_5103->setCargaHoraria(72);  DAS_5214->setCargaHoraria(108); DAS_5307->setCargaHoraria(72);
    DAS_5308->setCargaHoraria(72);  EEL_7540->setCargaHoraria(72);  INE_5108->setCargaHoraria(54);
    DAS_5109->setCargaHoraria(72);  DAS_5203->setCargaHoraria(90);  DAS_5320->setCargaHoraria(54);
    EEL_7550->setCargaHoraria(72);  EMC_5235->setCargaHoraria(72);  EMC_5425->setCargaHoraria(72);
    CNM_7820->setCargaHoraria(36);  DAS_5120->setCargaHoraria(108); DAS_5151->setCargaHoraria(72);
    DAS_5314->setCargaHoraria(72);  EEL_5193->setCargaHoraria(54);  EMC_5467->setCargaHoraria(54);
    EPS_2351->setCargaHoraria(72);
    DAS_5105->setCargaHoraria(108); DAS_5142->setCargaHoraria(72);  DAS_5318->setCargaHoraria(72);
    EEL_5354->setCargaHoraria(72);  EMC_5251->setCargaHoraria(72);  EMC_5258->setCargaHoraria(108);
    DAS_5402->setCargaHoraria(36);  DAS_5502->setCargaHoraria(216); EPS_7076->setCargaHoraria(54);
    OPT_PROF8->setCargaHoraria(144, false);
    OPT_PROF16->setCargaHoraria(288, false);
    OPT_LIVR->setCargaHoraria(36, false);
    DAS_5512->setCargaHoraria(360);

    // exigência de horas-aula obrigatórias já concluídas ("Pré CH" no currículo)
    DAS_5402->setPreCH(2300);
    EPS_7076->setPreCH(900);
    DAS_5512->setPreCH(3000);
}

void Widget::initGrupos()
{
    // grupos temáticos (cores em Diagram::corDoGrupo). Códigos corrigidos em
    // relação à anotação original: ECV5115 -> ECV5215, FS5113 -> FSC5113,
    // EMC5151 -> EMC5251, EEL5314 -> EEL5354, DAS6505 -> DAS5105 (Projeto
    // Integrador, em controle e em automação).
    const QStringList informatica{"informatica"}, controle{"controle"}, automacao{"automacao"},
            mecanica{"mecanica"}, eletrica{"eletrica"}, fisica{"fisica_calculo"},
            controleAutomacao{"controle", "automacao"};
    for(Diagram* d : {DAS_5334, DAS_5102, DAS_5332, DAS_5308, DAS_5320, DAS_5314})
        d->setGrupos(informatica);
    for(Diagram* d : {DAS_5210, DAS_5214, DAS_5109, DAS_5151, DAS_5120, DAS_5142})
        d->setGrupos(controle);
    for(Diagram* d : {DAS_5307, DAS_5203, DAS_5318})
        d->setGrupos(automacao);
    for(Diagram* d : {DAS_5412, DAS_5105, DAS_5502, DAS_5512})
        d->setGrupos(controleAutomacao);
    for(Diagram* d : {ECV_5215, EMC_5425, EMC_5235, EMC_5467, EMC_5258, EMC_5251})
        d->setGrupos(mecanica);
    for(Diagram* d : {EEL_7540, EEL_7550, EEL_5193, EEL_5354, EEL_5105})
        d->setGrupos(eletrica);
    for(Diagram* d : {FSC_5101, MTM_3110, FSC_5122, FSC_5002, MTM_3121, MTM_3120, MTM_3103,
                      MTM_3131, FSC_5113, DAS_5103, INE_5108})
        d->setGrupos(fisica);
}

int Widget::horasObrigatoriasConcluidas() const
{
    int total = 0;
    for(Diagram* d : diagrams)
        if(d->isActive() && d->obrigatoria)
            total += d->cargaHoraria;
    return total;
}
bool Widget::eventFilter(QObject *watched, QEvent *evt)
{
    if(watched == container && evt->type() == QEvent::Resize)
    {
        // as linhas cobrem exatamente o container
        for(Diagram* diagram : diagrams)
            for(ConnectingLine* l : diagram->lines)
                l->setGeometry(container->rect());
    }
    else if(view != nullptr && watched == view->viewport()
            && evt->type() == QEvent::MouseButtonPress && this->state == AddingPoints)
    {
        QMouseEvent* me = static_cast<QMouseEvent*>(evt);
        if(me->button() == Qt::LeftButton)
        {
            addPointAt(view->mapToScene(me->pos()).toPoint());
            return true; // não repassa o clique para os diagramas
        }
    }
    return QWidget::eventFilter(watched, evt);
}

QRectF Widget::faseRect(int firstCol, int lastCol)
{
    QRectF r;
    for(int c = firstCol; c <= lastCol; c++)
        for(int row = 0; row < gridLayout->rowCount(); row++)
            r |= QRectF(gridLayout->cellRect(row, c));
    return r;
}

QLayout* Widget::buildToolbar()
{
    QVBoxLayout* barras = new QVBoxLayout;
    barras->setSpacing(2);
    QHBoxLayout* bar = new QHBoxLayout;
    barras->addLayout(bar);
    bar->setSpacing(4);

    QToolButton* all = new QToolButton;
    all->setText("Ver tudo");
    all->setToolTip("Enquadrar todas as fases (Ctrl+0)");
    connect(all, &QToolButton::clicked, this, [this]() { view->fitAll(); });
    bar->addWidget(all);

    QToolButton* out = new QToolButton;
    out->setText(QString::fromUtf8("\u2212"));
    out->setToolTip("Diminuir zoom (Ctrl+-, ou Ctrl+roda do mouse)");
    connect(out, &QToolButton::clicked, this, [this]() { view->zoomOut(); });
    bar->addWidget(out);

    QToolButton* in = new QToolButton;
    in->setText("+");
    in->setToolTip("Aumentar zoom (Ctrl+=, ou Ctrl+roda do mouse)");
    connect(in, &QToolButton::clicked, this, [this]() { view->zoomIn(); });
    bar->addWidget(in);

    zoomLabel = new QLabel("100%");
    zoomLabel->setMinimumWidth(48);
    zoomLabel->setAlignment(Qt::AlignCenter);
    connect(view, &ZoomView::zoomChanged, this, [this](qreal s) {
        zoomLabel->setText(QString::number(qRound(s * 100)) + "%");
    });
    bar->addWidget(zoomLabel);
    bar->addSpacing(16);

    const int nFases = gridLayout->columnCount();
    for(int c = 0; c < nFases; c++)
    {
        QToolButton* b = new QToolButton;
        b->setText(QString::number(c + 1) + QString::fromUtf8("\u00aa"));
        b->setToolTip(QString::fromUtf8("Enquadrar a %1\u00aa fase (Shift+clique: intervalo de fases)").arg(c + 1));
        connect(b, &QToolButton::clicked, this, [this, c]() {
            bool shift = QApplication::keyboardModifiers() & Qt::ShiftModifier;
            if(shift)
                shiftAlone = false; // foi Shift+clique, não alterna o modo de edição
            bool range = shift && lastFase >= 0;
            if(range)
                view->focusColumns(faseRect(qMin(lastFase, c), qMax(lastFase, c)));
            else
            {
                view->focusColumns(faseRect(c, c));
                lastFase = c;
            }
        });
        bar->addWidget(b);
    }
    bar->addSpacing(16);
    horasLabel = new QLabel;
    horasLabel->setToolTip(QString::fromUtf8("Soma das horas-aula das disciplinas obrigatórias concluídas"));
    bar->addWidget(horasLabel);
    bar->addStretch();

    QLabel* hint = new QLabel(QString::fromUtf8("Ctrl+roda: zoom \u00b7 bot\u00e3o do meio: arrastar"));
    hint->setEnabled(false);
    bar->addWidget(hint);

    // "Colorir por": Situação (padrão) ou Grupo, com legenda própria para cada modo
    QHBoxLayout* legenda = new QHBoxLayout;
    legenda->setSpacing(10);
    legenda->addWidget(new QLabel("Colorir por:"));
    QToolButton* btSituacao = new QToolButton;
    btSituacao->setText(QString::fromUtf8("Situação"));
    btSituacao->setToolTip(QString::fromUtf8("Verde = concluída, azul = disponível, cinza = bloqueada"));
    QToolButton* btGrupo = new QToolButton;
    btGrupo->setText("Grupo");
    btGrupo->setToolTip(QString::fromUtf8("Cor da área de cada disciplina; clique num grupo para destacá-lo"));
    for(QToolButton* b : {btSituacao, btGrupo})
    {
        b->setCheckable(true);
        b->setAutoExclusive(true);
        legenda->addWidget(b);
    }
    btSituacao->setChecked(true);
    legenda->addSpacing(12);

    auto quadrado = [](const QColor& fundo, const QColor& borda = Qt::transparent) {
        QPixmap px(16, 16);
        px.fill(Qt::transparent);
        QPainter p(&px);
        p.setPen(borda == Qt::transparent ? Qt::NoPen : QPen(borda, 2));
        p.setBrush(fundo);
        p.drawRect(1, 1, 14, 14);
        return px;
    };
    auto item = [&](const QPixmap& px, const QString& texto, QWidget* pai) {
        QHBoxLayout* h = new QHBoxLayout;
        h->setSpacing(5);
        QLabel* ic = new QLabel; ic->setPixmap(px);
        h->addWidget(ic);
        h->addWidget(new QLabel(texto));
        static_cast<QHBoxLayout*>(pai->layout())->addLayout(h);
    };

    // legenda do modo Situação
    QWidget* legSituacao = new QWidget;
    legSituacao->setLayout(new QHBoxLayout);
    legSituacao->layout()->setContentsMargins(0, 0, 0, 0);
    legSituacao->layout()->setSpacing(16);
    item(quadrado(QColor("#1f9d55")), QString::fromUtf8("Concluída ✓"), legSituacao);
    item(quadrado(QColor("#2563eb")), QString::fromUtf8("Disponível para cursar"), legSituacao);
    item(quadrado(QColor("#e4e8ec"), QColor("#c3cad2")), "Bloqueada", legSituacao);
    legenda->addWidget(legSituacao);

    // legenda do modo Grupo: cada grupo é um botão que destaca só as suas disciplinas
    QWidget* legGrupo = new QWidget;
    QHBoxLayout* lg = new QHBoxLayout(legGrupo);
    lg->setContentsMargins(0, 0, 0, 0);
    lg->setSpacing(6);
    QList<QToolButton*> botoesGrupo;
    for(const QString& g : {"informatica", "controle", "automacao", "mecanica", "eletrica", "fisica_calculo"})
    {
        QToolButton* b = new QToolButton;
        b->setText(Diagram::nomeDoGrupo(g));
        b->setIcon(QIcon(quadrado(QColor(Diagram::corDoGrupo(g)))));
        b->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        b->setCheckable(true);
        b->setToolTip(QString::fromUtf8("Destacar só %1 (clique de novo para mostrar todos)").arg(Diagram::nomeDoGrupo(g)));
        b->setProperty("grupo", g);
        lg->addWidget(b);
        botoesGrupo << b;
    }
    QLabel* ambos = new QLabel;
    ambos->setPixmap([]() {
        QPixmap px(22, 16);
        px.fill(Qt::transparent);
        QPainter p(&px);
        QPolygon a, b;
        a << QPoint(0, 0) << QPoint(22, 0) << QPoint(0, 16);
        b << QPoint(22, 0) << QPoint(22, 16) << QPoint(0, 16);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(Diagram::corDoGrupo("controle"))); p.drawPolygon(a);
        p.setBrush(QColor(Diagram::corDoGrupo("automacao"))); p.drawPolygon(b);
        return px;
    }());
    lg->addSpacing(8);
    lg->addWidget(ambos);
    lg->addWidget(new QLabel(QString::fromUtf8("Controle + Automação")));
    QLabel* sem = new QLabel; sem->setPixmap(quadrado(QColor(Diagram::corDoGrupo(""))));
    lg->addWidget(sem);
    lg->addWidget(new QLabel("Sem grupo"));
    lg->addSpacing(12);
    lg->addWidget(new QLabel(QString::fromUtf8("<span style='color:#6b7480'>✓ concluída · esmaecida = bloqueada</span>")));
    legGrupo->hide();
    legenda->addWidget(legGrupo);
    legenda->addStretch();
    barras->addLayout(legenda);

    auto reaplicarTodos = [this]() {
        for(Diagram* d : diagrams)
            d->reaplicar();
    };
    connect(btSituacao, &QToolButton::toggled, this, [=](bool on) {
        if(!on) return;
        Diagram::modoGrupo = false;
        legGrupo->hide();
        legSituacao->show();
        reaplicarTodos();
    });
    connect(btGrupo, &QToolButton::toggled, this, [=](bool on) {
        if(!on) return;
        Diagram::modoGrupo = true;
        legSituacao->hide();
        legGrupo->show();
        reaplicarTodos();
    });
    for(QToolButton* b : botoesGrupo)
    {
        connect(b, &QToolButton::clicked, this, [=](bool marcado) {
            // destaque exclusivo: um grupo por vez; clicar de novo limpa
            for(QToolButton* o : botoesGrupo)
                if(o != b) o->setChecked(false);
            Diagram::grupoFoco = marcado ? b->property("grupo").toString() : QString();
            reaplicarTodos();
        });
    }

    connect(new QShortcut(QKeySequence("Ctrl+0"), this), &QShortcut::activated, this, [this]() { view->fitAll(); });
    connect(new QShortcut(QKeySequence("Ctrl+="), this), &QShortcut::activated, this, [this]() { view->zoomIn(); });
    connect(new QShortcut(QKeySequence::ZoomIn, this), &QShortcut::activated, this, [this]() { view->zoomIn(); });
    connect(new QShortcut(QKeySequence::ZoomOut, this), &QShortcut::activated, this, [this]() { view->zoomOut(); });
    return barras;
}
