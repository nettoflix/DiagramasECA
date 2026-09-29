#ifndef WIDGET_H
#define WIDGET_H

#include <QPainter>
#include <QScrollBar>
#include <QWidget>
//#include "diagram.h"
#include "connectingline.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPoint>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QCoreApplication>
#include <QFile>
#include <QTextStream>
#include <QScrollArea>
#include <QScrollBar>
#include <QGraphicsScene>
#include <QGraphicsProxyWidget>
#include <QLabel>
#include "zoomview.h"
#include "fasetitle.h"
class Diagram;
namespace Ui { class Widget; }

enum States {Off,WaitingFor, AddingPoints};
class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void setState(States state);
    States getState();
    void setCurrentDiagram(Diagram* diagram);
    void saveLines();
    void loadLines();
    bool carregarDisciplinas(const QString& caminho);
    QJsonObject entradaRoteador() const;
    void atualizarLinhas();
    void aplicarLinhas(const QJsonArray& linhas);
    int horasObrigatoriasConcluidas() const;
public slots:
    void checkPrerequisitesEvent();

private:
    QGraphicsScene* scene=nullptr;
    ZoomView* view=nullptr;
    QLabel* zoomLabel=nullptr;
    QLabel* horasLabel=nullptr;
    QLabel* linhasLabel=nullptr;
    QString linhasPath;
    QStringList ordemGrupos;  // grupos na ordem da seção [Grupos]
    int lastFase=-1;
    bool firstShow=true;
    bool shiftAlone=false;
    QRectF faseRect(int firstCol, int lastCol);
    QLayout* buildToolbar();
    void addPointAt(QPoint scenePos);
    QString path="";
    QGridLayout* gridLayout;

    int tempCounter =0;
    States state= States::WaitingFor;
    Diagram *currentDiagram = nullptr;
    Ui::Widget *ui;
    QVector<Diagram*> diagrams;
    ConnectingLine* fsc5101_to_fsc5002;
    ConnectingLine* mtm3110_to_fsc5002;

    Diagram* encheLinguica1;
    Diagram* encheLinguica2;
    Diagram* encheLinguica3;
    Diagram* encheLinguica4;
    Diagram* encheLinguica5;
    Diagram* encheLinguica6;
    Diagram* encheLinguica7;
    Diagram* encheLinguica8;
    Diagram* encheLinguica9;
    Diagram* encheLinguica10;
    Diagram* encheLinguica11;
    Diagram* encheLinguica12;

    QPoint FSC_5101_pos;
    QPoint FSC_5002_pos;
    bool waitingForClick;
    QPoint oldMousePos;
    bool use_oldH_mousePos=false;
     bool use_oldV_mousePos=false;
    void writeFile(QString fileName, QString content);
    QByteArray readFile(QString fileName);

    QWidget *container;
    int containerWidth=0;
    int containerHeight=0;

     ConnectingLine* line;
       int defaultSpaceWidth=0;
    void clearLines();
    bool eventFilter(QObject *watched, QEvent *evt) override;
protected:
       void paintEvent(QPaintEvent *) override;
       void showEvent(QShowEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;
        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent *event) override;




};
#endif // WIDGET_H
