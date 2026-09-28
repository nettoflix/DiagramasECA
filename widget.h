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
    void initPrerequisites();
    void initCargaHoraria();
    void initGrupos();
    int horasObrigatoriasConcluidas() const;
public slots:
    void checkPrerequisitesEvent();

private:
    QGraphicsScene* scene=nullptr;
    ZoomView* view=nullptr;
    QLabel* zoomLabel=nullptr;
    QLabel* horasLabel=nullptr;
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


    Diagram* MTM_3110;
    Diagram* MTM_3120;
    Diagram* FSC_5101;


    Diagram* DAS_5334;
    Diagram* DAS_5412;
    Diagram* ECZ_5102;
    Diagram*EGR_5606;
    //SEGUNDA FASE
    Diagram* EEL_5105;
    Diagram* DAS_5102;
    Diagram* MTM_3121;
    Diagram* FSC_5122;
    Diagram* FSC_5002;
    //TERCEIRA FASE
    Diagram* DAS_5332;
    Diagram* DAS_5210;
    Diagram* MTM_3131;
    Diagram* MTM_3103;
    Diagram* FSC_5113;
    Diagram* ECV_5215;
    //QUARTA FASE
    Diagram* DAS_5307;
    Diagram* DAS_5308;
    Diagram* DAS_5103;
    Diagram* DAS_5214;
    Diagram* EEL_7540;
    Diagram* INE_5108;
    //QUINTA FASE
    Diagram* DAS_5203;
    Diagram* DAS_5320;
    Diagram* EMC_5425;
    Diagram* DAS_5109;
    Diagram* EEL_7550;
    Diagram* EMC_5235;
    //SEXTA FASE
    Diagram* CNM_7820;
    Diagram* DAS_5314;
    Diagram* EMC_5467;
    Diagram* DAS_5120;
    Diagram* EEL_5193;
    Diagram* DAS_5151;
    Diagram* EPS_2351;
    //SÉTIMA FASE
    Diagram* EMC_5258;
    Diagram* DAS_5105;
    Diagram* DAS_5142;
    Diagram* EMC_5251;
    Diagram* EEL_5354;
    Diagram* DAS_5318;
    //OITAVA FASE
    Diagram* DAS_5502;
    Diagram* DAS_5402;
    Diagram* OPT_PROF8;
    Diagram* EPS_7076;
    //NONA FASE
    Diagram* OPT_PROF16;
    Diagram* OPT_LIVR;
    //DÉCIMA
    Diagram* DAS_5512;







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
