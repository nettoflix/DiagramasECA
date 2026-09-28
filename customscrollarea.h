#ifndef CUSTOMSCROLLAREA_H
#define CUSTOMSCROLLAREA_H

#include <QObject>
#include <QScrollArea>
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>

class CustomScrollArea : public QScrollArea {
    Q_OBJECT

public:
    explicit CustomScrollArea(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event);
    bool eventFilter(QObject *watched, QEvent *evt) override;

private:
    QPoint mousePos;
};

#endif // CUSTOMSCROLLAREA_H
