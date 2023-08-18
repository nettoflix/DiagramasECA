#ifndef DIAGRAM_H
#define DIAGRAM_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QHash>
#include <QStyleFactory>
#include "connectingline.h"
#include "widget.h"
#include <QLabel>
#include "myconstants.h"
namespace Ui {
class Diagram;
}
//class Widget;
class Diagram : public QPushButton
{
    Q_OBJECT

public:
    explicit Diagram(QWidget *parent = nullptr,QWidget *mainWidget = nullptr ,QString name="");
    ~Diagram();
   // QPushButton* button;
    Ui::Diagram *ui;
    QWidget *mainWidget;
    QWidget *parent;
    QString name;
    int lineIndex = 0;
    bool adicionandoPontos = false;
    bool isActive () const;
    bool isOpen() const;
    void setPrerequisites( QVector<Diagram*>* prerequisites);
    void addPointToLine(QPoint point);
    void addNewLine();
    void incIndex();
    void decIndex();
    QList<QPoint>* getCurrentLine();
    bool operator==(const Diagram &other) const;
    uint myQHash(const Diagram &key);
     QVector<Diagram*>* prerequisites= nullptr;
     QVector<ConnectingLine*> lines;
signals:
    void checkPrerequisites();
public slots:
    void setActive_slot();
    void setActive(bool set);
    void buildLines();
    void paintDiagramColor(QString color);
    void paintDiagramLines(Qt::GlobalColor color);
protected:

   // void paintEvent(QPaintEvent *e) override;

private:
    bool active=false;
};

#endif // DIAGRAM_H
