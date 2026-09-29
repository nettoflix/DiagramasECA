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
#include <QMessageBox>
#include <QRegularExpression>
#include <QSet>
#include <QJSEngine>
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QCryptographicHash>
#include <QElapsedTimer>




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
    linhasPath = QCoreApplication::applicationDirPath() + "/files/linhas.json";
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

    gridLayout->setHorizontalSpacing(this->defaultSpaceWidth);
    // dentro do QGraphicsProxyWidget o container vira janela de topo, e o estilo
    // usaria margem 11 (de janela) em vez de 9 (de widget filho): fixa em 9 para
    // manter a geometria com que files/saved.txt foi gerado
    gridLayout->setContentsMargins(9, 9, 9, 9);
    gridLayout->setVerticalSpacing(6);

    // disciplinas, fases, pré-requisitos, horas-aula e grupos vêm de
    // files/disciplinas.txt na pasta do executável (sem ele, da cópia
    // embutida nos recursos)
    QString disciplinasPath = QCoreApplication::applicationDirPath() + "/files/disciplinas.txt";
    if(!QFile::exists(disciplinasPath))
        disciplinasPath = ":/files/disciplinas.txt";
    carregarDisciplinas(disciplinasPath);

    // barra de zoom no topo (depois da grade pronta: um botão por fase/coluna)
    mainLayout->insertLayout(0, buildToolbar());

    // tamanho natural da grade (sem esticar), igual ao usado para gerar saved.txt
    container->adjustSize();
    gridLayout->activate(); // posiciona as caixas já agora: o roteador usa a geometria delas
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
    atualizarLinhas();

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
        bool isActive = diagramObj.value("isActive").toBool();
        if(isActive) diagram->setActive(true);

        // as linhas não vêm mais daqui: são calculadas pelo roteador
        // (atualizarLinhas); de saved.txt só se usa o progresso
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
// Lê as disciplinas (formato descrito no cabeçalho de files/disciplinas.txt),
// cria um Diagram para cada uma e o coloca na grade: a fase N é a coluna N-1,
// com o título da fase na linha 0. Erros de formato são avisados numa caixa
// de diálogo e a linha com erro é ignorada.
bool Widget::carregarDisciplinas(const QString& caminho)
{
    QStringList erros;
    QFile file(caminho);
    if(!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this, "Fluxograma ECA",
                              QString::fromUtf8("Não foi possível abrir %1").arg(caminho));
        return false;
    }
    QTextStream in(&file);
    in.setCodec("UTF-8");

    static const QStringList gruposConhecidos{"informatica", "controle", "automacao",
                                              "mecanica", "eletrica", "fisica_calculo"};
    const QRegularExpression cabecalhoFase("^\\[\\s*fase\\s+(\\d+)\\s*\\]$",
                                           QRegularExpression::CaseInsensitiveOption);
    struct Pendente { Diagram* diagram; QStringList pre; int numLinha; };
    QVector<Pendente> pendentes;
    QHash<QString, Diagram*> porCodigo;
    QSet<QString> nomes;
    QHash<int, int> ultimaLinha; // fase -> linha da grade da última disciplina
    int fase = 0;
    int numLinha = 0;
    auto erro = [&](int n, const QString& msg) {
        erros << QString("linha %1: %2").arg(n).arg(msg);
    };
    auto lista = [](const QString& valor) {
        QStringList l;
        for(const QString& item : valor.split(',', QString::SkipEmptyParts))
            if(!item.trimmed().isEmpty())
                l << item.trimmed();
        return l;
    };

    while(!in.atEnd())
    {
        const QString linha = in.readLine().trimmed();
        numLinha++;
        if(linha.isEmpty() || linha.startsWith('#'))
            continue;

        QRegularExpressionMatch m = cabecalhoFase.match(linha);
        if(m.hasMatch())
        {
            fase = m.captured(1).toInt();
            if(fase < 1)
                erro(numLinha, "fase deve ser 1 ou maior");
            else if(gridLayout->itemAtPosition(0, fase - 1) == nullptr)
                gridLayout->addWidget(new FaseTitle(this, QString::fromUtf8("%1º fase").arg(fase)), 0, fase - 1);
            continue;
        }
        if(fase < 1)
        {
            erro(numLinha, QString::fromUtf8("disciplina antes de um cabeçalho [Fase N]"));
            continue;
        }

        QStringList campos = linha.split('|');
        for(QString& c : campos)
            c = c.trimmed();
        if(campos.size() < 2 || campos[0].isEmpty() || campos[1].isEmpty())
        {
            erro(numLinha, QString::fromUtf8("esperado \"CÓDIGO | Nome | ha: ...\""));
            continue;
        }
        const QString codigo = campos[0], nome = campos[1];
        if(porCodigo.contains(codigo))
        {
            erro(numLinha, QString::fromUtf8("código repetido: %1").arg(codigo));
            continue;
        }
        if(nomes.contains(nome))
        {
            erro(numLinha, QString::fromUtf8("nome repetido: %1").arg(nome));
            continue;
        }

        int ha = -1, preCH = 0, linhaGrade = ultimaLinha.value(fase, 0) + 1;
        bool obrigatoria = true, valido = true;
        QStringList pre, grupos;
        for(int i = 2; i < campos.size(); i++)
        {
            const QString chave = campos[i].section(':', 0, 0).trimmed();
            const QString valor = campos[i].section(':', 1).trimmed();
            bool numero = true;
            if(chave == "ha")
                ha = valor.toInt(&numero);
            else if(chave == "linha")
                linhaGrade = valor.toInt(&numero);
            else if(chave == "preCH")
                preCH = valor.toInt(&numero);
            else if(chave == "pre")
                pre = lista(valor);
            else if(chave == "grupos")
                grupos = lista(valor);
            else if(chave == "optativa" && valor.isEmpty())
                obrigatoria = false;
            else
            {
                erro(numLinha, QString::fromUtf8("campo desconhecido: %1").arg(campos[i]));
                valido = false;
            }
            if(!numero)
            {
                erro(numLinha, QString::fromUtf8("número inválido em \"%1\"").arg(campos[i]));
                valido = false;
            }
        }
        for(const QString& g : grupos)
            if(!gruposConhecidos.contains(g))
            {
                erro(numLinha, QString::fromUtf8("grupo desconhecido: %1").arg(g));
                valido = false;
            }
        if(ha < 0)
        {
            erro(numLinha, QString::fromUtf8("falta a carga horária (ha: N)"));
            valido = false;
        }
        if(linhaGrade < 1)
        {
            erro(numLinha, "a linha da grade deve ser 1 ou maior");
            valido = false;
        }
        else if(gridLayout->itemAtPosition(linhaGrade, fase - 1) != nullptr)
        {
            erro(numLinha, QString::fromUtf8("a linha %1 da fase %2 já está ocupada").arg(linhaGrade).arg(fase));
            valido = false;
        }
        if(!valido)
            continue;

        Diagram* d = new Diagram(container, this, nome);
        d->codigo = codigo;
        d->setCargaHoraria(ha, obrigatoria);
        if(preCH > 0)
            d->setPreCH(preCH);
        if(!grupos.isEmpty())
            d->setGrupos(grupos);
        gridLayout->addWidget(d, linhaGrade, fase - 1);
        diagrams.append(d);
        porCodigo.insert(codigo, d);
        nomes.insert(nome);
        ultimaLinha[fase] = linhaGrade;
        if(!pre.isEmpty())
            pendentes.append({d, pre, numLinha});
    }

    // pré-requisitos só depois de ler tudo: podem citar qualquer disciplina
    for(const Pendente& p : pendentes)
    {
        QVector<Diagram*>* prerequisites = new QVector<Diagram*>;
        for(const QString& codigo : p.pre)
        {
            if(porCodigo.contains(codigo))
                prerequisites->append(porCodigo.value(codigo));
            else
                erro(p.numLinha, QString::fromUtf8("pré-requisito desconhecido: %1").arg(codigo));
        }
        p.diagram->setPrerequisites(prerequisites);
    }

    if(!erros.isEmpty())
    {
        qWarning().noquote() << caminho << "\n" << erros.join("\n");
        QMessageBox::warning(this, "Fluxograma ECA",
                             QString::fromUtf8("Problemas em %1 (as linhas com erro foram ignoradas):\n\n%2")
                             .arg(caminho, erros.join("\n")));
    }
    return erros.isEmpty();
}

// Entrada do roteador (web/roteador.js): geometria real de cada caixa na
// grade e as relações de pré-requisito.
QJsonObject Widget::entradaRoteador() const
{
    QJsonObject caixas;
    QJsonArray arestas;
    for(Diagram* d : diagrams)
    {
        int linha, coluna, rs, cs;
        gridLayout->getItemPosition(gridLayout->indexOf(d), &linha, &coluna, &rs, &cs);
        caixas.insert(d->codigo, QJsonObject{{"x", d->x()}, {"y", d->y()}, {"w", d->width()},
                                             {"h", d->height()}, {"linha", linha}, {"coluna", coluna}});
        if(d->prerequisites != nullptr)
            for(Diagram* p : *d->prerequisites)
                arestas.append(QJsonArray{p->codigo, d->codigo});
    }
    return QJsonObject{{"caixas", caixas}, {"arestas", arestas}, {"margemTopo", 50}};
}

// Roda o roteador para uma semente, numa thread do QtConcurrent; cada chamada
// tem o seu próprio motor JS, então as sementes rodam em paralelo.
struct RotearSemente
{
    typedef QString result_type;
    QString codigo, entrada;
    int iteracoes;
    QString operator()(int semente) const
    {
        const QJsonObject opcoes{{"iteracoes", iteracoes}, {"sementes", QJsonArray{semente}}};
        QJSEngine engine;
        QJSValue r = engine.evaluate(codigo, "roteador.js");
        if(!r.isError())
            r = engine.globalObject().property("Roteador").property("rotearJSON")
                    .call({entrada, QString(QJsonDocument(opcoes).toJson(QJsonDocument::Compact))});
        return r.isError() ? "ERRO: " + r.toString() : r.toString();
    }
};

// As linhas são calculadas por web/roteador.js (o mesmo código da versão web)
// e guardadas em files/linhas.json junto com a entrada que as gerou: só
// recalcula quando as disciplinas, as posições ou o roteador mudam.
void Widget::atualizarLinhas()
{
    QFile arquivo(":/web/roteador.js");
    arquivo.open(QFile::ReadOnly);
    const QByteArray codigo = arquivo.readAll();
    const QJsonObject entrada = entradaRoteador();
    const QJsonObject opcoes{{"iteracoes", 60000}, {"sementes", QJsonArray{1, 2, 3}}};
    const QString versao = QCryptographicHash::hash(codigo, QCryptographicHash::Sha1).toHex();

    const QJsonObject cache = QJsonDocument::fromJson(readFile(linhasPath)).object();
    if(cache.value("roteador").toString() == versao && cache.value("entrada").toObject() == entrada
            && cache.value("opcoes").toObject() == opcoes)
    {
        aplicarLinhas(cache.value("linhas").toArray());
        return;
    }

    linhasLabel->setText(QString::fromUtf8("Calculando as linhas…"));
    linhasLabel->show();
    QElapsedTimer* tempo = new QElapsedTimer;
    tempo->start();
    QFutureWatcher<QString>* watcher = new QFutureWatcher<QString>(this);
    connect(watcher, &QFutureWatcher<QString>::finished, this, [=]() {
        const QList<QString> resultados = watcher->future().results();
        watcher->deleteLater();
        qDebug() << "roteador:" << tempo->elapsed() << "ms";
        delete tempo;
        // fica com a semente de menor custo (a primeira, em caso de empate)
        QJsonObject r;
        for(const QString& resultado : resultados)
        {
            if(resultado.startsWith("ERRO"))
            {
                qWarning().noquote() << resultado;
                linhasLabel->setText(QString::fromUtf8("Erro ao calcular as linhas"));
                linhasLabel->setToolTip(resultado);
                return;
            }
            QJsonObject o = QJsonDocument::fromJson(resultado.toUtf8()).object();
            if(r.isEmpty() || o.value("custo").toDouble() < r.value("custo").toDouble())
                r = o;
        }
        aplicarLinhas(r.value("linhas").toArray());
        linhasLabel->hide();
        QJsonObject novo{{"roteador", versao}, {"entrada", entrada}, {"opcoes", opcoes},
                         {"relatorio", r.value("relatorio")}, {"linhas", r.value("linhas")}};
        writeFile(linhasPath, QJsonDocument(novo).toJson(QJsonDocument::Compact));
    });
    QList<int> sementes;
    for(const QJsonValue& s : opcoes.value("sementes").toArray())
        sementes << s.toInt();
    RotearSemente tarefa{QString::fromUtf8(codigo), QString(QJsonDocument(entrada).toJson(QJsonDocument::Compact)),
                         opcoes.value("iteracoes").toInt()};
    watcher->setFuture(QtConcurrent::mapped(sementes, tarefa));
}

// linhas: [{de, para, pts: [[x, y], ...]}]; cada linha pertence à disciplina de origem
void Widget::aplicarLinhas(const QJsonArray& linhas)
{
    QHash<QString, Diagram*> porCodigo;
    for(Diagram* d : diagrams)
    {
        porCodigo.insert(d->codigo, d);
        for(ConnectingLine* l : d->lines)
            l->clearPoints();
        d->lineIndex = 0;
    }
    for(const QJsonValue& v : linhas)
    {
        const QJsonObject o = v.toObject();
        Diagram* d = porCodigo.value(o.value("de").toString());
        if(d == nullptr)
            continue;
        if(d->lineIndex >= d->lines.size())
        {
            ConnectingLine* nova = new ConnectingLine(container);
            nova->setGeometry(container->rect());
            nova->show();
            d->lines.append(nova);
        }
        ConnectingLine* l = d->lines[d->lineIndex++];
        for(const QJsonValue& p : o.value("pts").toArray())
            l->addPoint(QPoint(p.toArray().at(0).toInt(), p.toArray().at(1).toInt()));
    }
    // recolore as linhas (verde se a origem está concluída)
    for(Diagram* d : diagrams)
        if(d->isActive())
            d->setActive(true);
    checkPrerequisitesEvent();
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
    bar->addSpacing(16);
    linhasLabel = new QLabel;
    linhasLabel->hide();
    bar->addWidget(linhasLabel);
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
