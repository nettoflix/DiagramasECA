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
public slots:
    void checkPrerequisitesEvent();

private:
    qreal scaleFactor=1;
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
    Diagram* DAS_5411;
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
    Diagram* DAS_5114;
    Diagram* EEL_7540;
    Diagram* INE_5108;
    //QUINTA FASE
    Diagram* DAS_5203;
    Diagram* DAS_5312;
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
    //SÉTIMA FASE
    Diagram* EMC_5258;
    Diagram* DAS_5104;
    Diagram* DAS_5142;
    Diagram* EMC_5251;
    Diagram* EEL_5354;
    Diagram* DAS_5310;
    //OITAVA FASE
    Diagram* DAS_5501;
    Diagram* DAS_5401;
    Diagram* OPT_PROF8;
    Diagram* EPS_5211;
    //NONA FASE
    Diagram* OPT_PROF16;
    Diagram* OPT_LIVR;
    //DÉCIMA
    Diagram* DAS_5511;







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
    QScrollBar* verticalScrollBar;
    QScrollBar* horizontalScrollBar;
    QPoint oldMousePos;
    bool use_oldH_mousePos=false;
     bool use_oldV_mousePos=false;
    void writeFile(QString fileName, QString content);
    QByteArray readFile(QString fileName);


    int containerWidth=0;
    int containerHeight=0;
         bool eventFilter(QObject *watched, QEvent *evt);
protected:
       void paintEvent(QPaintEvent *) override;
       void showEvent(QShowEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;
        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;
        void wheelEvent(QWheelEvent *event) override;


};
#endif // WIDGET_H
