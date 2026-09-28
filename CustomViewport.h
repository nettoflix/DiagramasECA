#ifndef CUSTOMVIEWPORT_H
#define CUSTOMVIEWPORT_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

class CustomViewport : public QWidget {
    Q_OBJECT

public:
    explicit CustomViewport(QWidget *parent = nullptr) : QWidget(parent) {
        setMouseTracking(true);
    }

protected:
    void mouseMoveEvent(QMouseEvent *event) override {
        qDebug("customViewport mouseMoveEvent(QMouseEvent *event) ");
        mousePos = event->pos(); // Update mouse position
        update();                // Request a repaint
    }

    void paintEvent(QPaintEvent *event) override {
             qDebug("customViewport paintEvent(QMouseEvent *event) ");
        QPainter painter(this);
        painter.setPen(Qt::blue);
        painter.drawText(mousePos, QString("Mouse at: [%1, %2]").arg(mousePos.x()).arg(mousePos.y()));
    }

private:
    QPoint mousePos; // Store the mouse position
};

#endif // CUSTOMVIEWPORT_H
