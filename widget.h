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
public slots:
    void checkPrerequisitesEvent();

private:
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
protected:
       void paintEvent(QPaintEvent *) override;
       void showEvent(QShowEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;
        void keyPressEvent(QKeyEvent* event) override;
        void keyReleaseEvent(QKeyEvent *event) override;
        void mousePressEvent(QMouseEvent *event) override;

};
#endif // WIDGET_H
