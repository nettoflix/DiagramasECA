#include "diagram.h"
#include "./ui_diagram.h"
#include <QDebug>
#include <QPalette>
#include <QFontMetrics>


class Widget;
Diagram::Diagram(QWidget *parent, QWidget *mainWidget, QString name) :
    QPushButton(parent),
    ui(new Ui::Diagram)
{
    ui->setupUi(this);
    this->mainWidget = mainWidget;//dynamic_cast<Widget*>(parent);
    this->parent = parent;
    this->name = name;
    this->paintDiagramColor(MyConstants::my_green);
    //this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    this->setFixedSize(159,159);
    //this->setMinimumSize(50,40);
  //  this->setMaximumHeight(300);

   // QVBoxLayout* layout = new QVBoxLayout(this);  // Create a vertical layout
    //layout->setContentsMargins(5, 0, 5, 0);


    //QLabel* label = new QLabel(this);

    QLabel* label = ui->label;
    label->setStyleSheet("border: none;");
    //QFont f( "Arial", 11, QFont::ExtraBold);
    //label->setFont( f);
    label->setWordWrap(true); //break line if text is too big

    label->setText(name);
    label->setAlignment(Qt::AlignCenter);
    //layout->addWidget(label);

    //ui->label->setText(name);
    connect(this, SIGNAL(clicked()), this, SLOT(setActive_slot()));
    connect(this, SIGNAL(checkPrerequisites()), mainWidget, SLOT(checkPrerequisitesEvent()));
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
    delete prerequisites;
}

 bool Diagram::isActive() const
{
    return this->active;
}

int Diagram::faltamHoras() const
{
    if(preCH <= 0)
        return 0;
    Widget* w = dynamic_cast<Widget*>(mainWidget);
    if(w == nullptr)
        return 0;
    // as horas da própria disciplina não contam para liberá-la
    int horas = w->horasObrigatoriasConcluidas();
    if(active && obrigatoria)
        horas -= cargaHoraria;
    return qMax(0, preCH - horas);
}

bool Diagram::isOpen() const
{
    bool open = faltamHoras() == 0;
       // qDebug()<< "ISS OPEN?:";
        if(prerequisites != nullptr)
           {
               //QList<Diagram*> keys = prerequisites->value();

               for (Diagram* prerequisite : *prerequisites)
               {
                   if(!prerequisite->isActive())
                   {
                    open=false;
                    //prerequisite->setColor(Qt::blue);
                   // qDebug()<< "pre requisite in" <<this->name <<"is NOT active!";
                    }
               }
           }
            // sem pré-requisitos: vale só a exigência de carga horária
            return open;
}

void Diagram::setPrerequisites(QVector<Diagram*> *prerequisites)
{
    this->prerequisites = prerequisites;
    atualizarTexto();
}

void Diagram::setCargaHoraria(int horasAula, bool obrigatoria)
{
    this->cargaHoraria = horasAula;
    this->obrigatoria = obrigatoria;
    atualizarTexto();
}

void Diagram::setPreCH(int horasAula)
{
    this->preCH = horasAula;
    atualizarTexto();
}

void Diagram::atualizarTexto()
{
    QString info = QString::number(cargaHoraria) + " h/a";
    if(!obrigatoria)
        info += " · optativa";
    // nomes longos: reduz a fonte até caberem em 3 linhas, sobrando espaço
    // para a linha de horas-aula
    QFont fonte = ui->label->font();
    int pontos = fonte.pointSize();
    const int larguraUtil = width() - 30;
    while(pontos > 9)
    {
        fonte.setPointSize(pontos);
        QFontMetrics fm(fonte);
        QRect r = fm.boundingRect(QRect(0, 0, larguraUtil, 1000), Qt::TextWordWrap | Qt::AlignCenter, name);
        int palavraMaisLarga = 0;
        for(const QString& palavra : name.split(' ', QString::SkipEmptyParts))
            palavraMaisLarga = qMax(palavraMaisLarga, fm.width(palavra));
        if(r.height() <= 3 * fm.lineSpacing() && palavraMaisLarga <= larguraUtil)
            break;
        pontos--;
    }
    QString html = QString("<span style='font-size:%1pt;'>").arg(pontos) + name.toHtmlEscaped() + "</span>"
            + "<br><span style='font-size:9pt; font-weight:normal;'>" + info;
    if(preCH > 0)
        html += "<br>requer " + QString::number(preCH) + " h/a obrig.";
    html += "</span>";
    ui->label->setText(html);

    QString dica = name + "\n" + QString::number(cargaHoraria) + " horas-aula"
            + (obrigatoria ? "" : " (optativa)");
    if(prerequisites != nullptr && !prerequisites->isEmpty())
    {
        QStringList nomes;
        for(Diagram* p : *prerequisites)
            nomes << p->name;
        dica += "\nPré-requisitos: " + nomes.join(", ");
    }
    if(preCH > 0)
        dica += "\nExige " + QString::number(preCH) + " horas-aula obrigatórias concluídas";
    setToolTip(dica);
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

void Diagram::decIndex()
{
  this->lineIndex--;
}

QList<QPoint>* Diagram::getCurrentLine()
{
 return this->lines[lineIndex]->points;
}

bool Diagram::operator==(const Diagram &other) const
{
    return other.objectName() == this->objectName();
}

uint Diagram::myQHash(const Diagram &key)
{
return qHash(key.objectName());
}

void Diagram::setActive_slot()
{
    //qDebug() << this->pos();
    if(this->isOpen())
    {
        if(this->active)
        {
            setActive(false);
        }
        else
        {
            setActive(true);
        }
        emit checkPrerequisites();

    }

}

void Diagram::setActive(bool set)
{
    if(!set) //desativando
    {
        this->active = false;
        this->paintDiagramColor(MyConstants::my_blue);

        this->paintDiagramLines(Qt::red);
       // paintDiagramColorAndLines(false);
    }
    else //ativando
    {
        this->active=true;
        this->paintDiagramColor(MyConstants::my_green);
        this->paintDiagramLines(Qt::blue);
       // paintDiagramColorAndLines(true);
    }
}
void Diagram:: paintDiagramColor(QString colorHex)
{
   // QString borderColor = "#FF0000"; // Hex color code for red
       QString colorStr = QString("%1").arg(colorHex);
       this->setStyleSheet("background-color:" + colorStr+ "; margin-top: 50px; border: 3px solid black");
}
void Diagram:: paintDiagramLines(Qt::GlobalColor color)
{
    for(ConnectingLine *line : this->lines)
    {
        line->setColor(color);
    }
}



//void Diagram::paintEvent(QPaintEvent *e)
//{

    //QPainter painter(this);
  //  painter.setTransform(QTransform().scale(scaleFactor, scaleFactor));
    // Draw your widgets and contents here
   // QWidget::paintEvent(e);
//}


void Diagram::buildLines()
{
        Widget* l_parent = dynamic_cast<Widget*>(mainWidget);
        if(l_parent->getState() == States::WaitingFor) //state is waitingFlorClick
        {
            l_parent->setState(States::AddingPoints);
            l_parent->setCurrentDiagram(this);
            //adicionandoPontos = true;
            qDebug() << "BuldingLines";
            qDebug() << this->height()/2;
            //adiciona o ponto inicial da linha perto do diagrama

        }



}

