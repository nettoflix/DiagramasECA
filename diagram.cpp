#include "diagram.h"
#include "./ui_diagram.h"
#include <QDebug>
#include <QPalette>
#include <QFontMetrics>
#include <QColor>
#include <QMap>


class Widget;
Diagram::Diagram(QWidget *parent, QWidget *mainWidget, QString name) :
    QPushButton(parent),
    ui(new Ui::Diagram)
{
    ui->setupUi(this);
    this->mainWidget = mainWidget;//dynamic_cast<Widget*>(parent);
    this->parent = parent;
    this->name = name;
    //this->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    this->setFixedSize(159,159);
    //this->setMinimumSize(50,40);
  //  this->setMaximumHeight(300);

   // QVBoxLayout* layout = new QVBoxLayout(this);  // Create a vertical layout
    //layout->setContentsMargins(5, 0, 5, 0);


    //QLabel* label = new QLabel(this);

    QLabel* label = ui->label;
    label->setStyleSheet("border: none;");
    // o texto não pode ficar com o clique: dentro do QGraphicsProxyWidget (zoom)
    // o "release" ficaria preso no QLabel e o botão nunca emitiria clicked()
    label->setAttribute(Qt::WA_TransparentForMouseEvents);
    //QFont f( "Arial", 11, QFont::ExtraBold);
    //label->setFont( f);
    label->setWordWrap(true); //break line if text is too big

    label->setText(name);
    label->setAlignment(Qt::AlignCenter);
    //layout->addWidget(label);

    //ui->label->setText(name);
    aplicarEstado(Disponivel);
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
    QString info = QString(estado == Concluida ? QString::fromUtf8("✓ ") : "") + QString::number(cargaHoraria) + " h/a";
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
    if(!grupos.isEmpty())
    {
        QStringList nomes;
        for(const QString& g : grupos)
            nomes << nomeDoGrupo(g);
        dica += "\nGrupo: " + nomes.join(" + ");
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
        this->aplicarEstado(Disponivel);

        this->paintDiagramLines(QColor("#a3acb6")); // cinza
       // paintDiagramColorAndLines(false);
    }
    else //ativando
    {
        this->active=true;
        this->aplicarEstado(Concluida);
        this->paintDiagramLines(QColor("#13824a")); // verde, como o contorno de concluída
       // paintDiagramColorAndLines(true);
    }
}
QString Diagram::corDoGrupo(const QString& grupo)
{
    static const QMap<QString, QString> cores = {
        {"informatica",    "#b9a5e6"},   // lavanda
        {"controle",       "#76c9bd"},   // verde-água
        {"automacao",      "#f3b75c"},   // âmbar
        {"mecanica",       "#e79b87"},   // terracota
        {"eletrica",       "#efdb6c"},   // amarelo
        {"fisica_calculo", "#9dc0e7"},   // azul-céu
    };
    return cores.value(grupo, "#dfe3e8"); // sem grupo: cinza neutro
}

QString Diagram::nomeDoGrupo(const QString& grupo)
{
    static const QMap<QString, QString> nomes = {
        {"informatica", QString::fromUtf8("Informática")}, {"controle", "Controle"},
        {"automacao", QString::fromUtf8("Automação")}, {"mecanica", QString::fromUtf8("Mecânica")},
        {"eletrica", QString::fromUtf8("Elétrica")}, {"fisica_calculo", QString::fromUtf8("Física e Cálculo")},
    };
    return nomes.value(grupo, grupo);
}

void Diagram::setGrupos(const QStringList& grupos)
{
    this->grupos = grupos;
    aplicarEstado(estado);
    atualizarTexto();
}

// mistura a cor com branco (0 = cor pura, 1 = branco): usado para esmaecer as bloqueadas
static QString clarear(const QString& hex, double t)
{
    QColor c(hex);
    return QColor(int(c.red() + (255 - c.red()) * t),
                  int(c.green() + (255 - c.green()) * t),
                  int(c.blue() + (255 - c.blue()) * t)).name();
}

bool Diagram::modoGrupo = false;
QString Diagram::grupoFoco;

void Diagram::aplicarEstado(Estado estado)
{
    this->estado = estado;
    QString fundo, borda, corTexto;
    if(!modoGrupo)
    {
        // modo Situação: uma cor forte por estado, sem grupos
        switch(estado)
        {
        case Concluida:
            fundo = "background-color: #1f9d55;"; borda = "border: 3px solid #157a41;"; corTexto = "#ffffff"; break;
        case Disponivel:
            fundo = "background-color: #2563eb;"; borda = "border: 3px solid #1a47b8;"; corTexto = "#ffffff"; break;
        case Bloqueada:
            fundo = "background-color: #e4e8ec;"; borda = "border: 2px solid #c3cad2;"; corTexto = "#7a838e"; break;
        }
    }
    else
    {
        // modo Grupo: cor do grupo (diagonal com dois grupos); o estado aparece
        // só de leve (bloqueadas esmaecidas, ✓ nas concluídas)
        const bool fora = !grupoFoco.isEmpty() && !grupos.contains(grupoFoco);
        QStringList cores;
        for(const QString& g : (grupos.isEmpty() ? QStringList{""} : grupos))
            cores << clarear(corDoGrupo(g), fora ? 0.85 : (estado == Bloqueada ? 0.6 : 0.0));
        if(cores.size() == 1)
            fundo = "background-color: " + cores[0] + ";";
        else
        {
            QStringList stops;
            for(int i = 0; i < cores.size(); i++)
            {
                double a = double(i) / cores.size(), b = double(i + 1) / cores.size();
                stops << QString("stop:%1 %2").arg(i == 0 ? 0.0 : a + 0.001).arg(cores[i])
                      << QString("stop:%1 %2").arg(b).arg(cores[i]);
            }
            fundo = "background: qlineargradient(x1:0, y1:0, x2:1, y2:1, " + stops.join(", ") + ");";
        }
        if(fora)
            borda = "border: 2px solid #d5dbe1;";
        else if(estado == Concluida)
            borda = "border: 3px solid #2b333d;";
        else
            borda = "border: 2px solid #9aa3ad;";
        corTexto = (fora || estado == Bloqueada) ? "#8a939e" : "#16202a";
    }
    // seletor pela classe: o QLabel filho não herda o fundo (senão o
    // gradiente recomeçaria dentro dele)
    setStyleSheet("Diagram { " + fundo + " margin-top: 50px; " + borda + " }");
    ui->label->setStyleSheet("border: none; background: transparent; margin-top: 50px; color: " + corTexto + ";");
    atualizarTexto();
}

void Diagram:: paintDiagramColor(QString colorHex)
{
   // QString borderColor = "#FF0000"; // Hex color code for red
       QString colorStr = QString("%1").arg(colorHex);
       this->setStyleSheet("background-color:" + colorStr+ "; margin-top: 50px; border: 3px solid black");
}
void Diagram:: paintDiagramLines(const QColor& color)
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

