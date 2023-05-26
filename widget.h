#ifndef WIDGET_H
#define WIDGET_H

#include <QPainter>
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
    Diagram* FSC_5002;
    QPoint FSC_5101_pos;
    QPoint FSC_5002_pos;
    bool waitingForClick;
    void writeFile(QString fileName, QString content);
    QByteArray readFile(QString fileName);
protected:
       void paintEvent(QPaintEvent *) override;
       void showEvent(QShowEvent *event) override;
        void resizeEvent(QResizeEvent *event) override;
        void keyPressEvent(QKeyEvent* event) override;
        void mousePressEvent(QMouseEvent *event) override;
};
#endif // WIDGET_H
