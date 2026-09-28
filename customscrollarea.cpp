#include "customscrollarea.h"
CustomScrollArea::CustomScrollArea(QWidget *parent) : QScrollArea(parent) {
    setMouseTracking(true); // Enable mouse tracking
    viewport()->setMouseTracking(true);
    viewport()->installEventFilter(this);
}
bool CustomScrollArea::eventFilter(QObject *watched, QEvent *event) {
    if (watched == viewport()) {
        qDebug("CustomScrollArea::eventFilter");
        if (event->type() == QEvent::Paint) {
             qDebug("CustomScrollArea::eventFilter 2");
            QPaintEvent *paintEvent = static_cast<QPaintEvent *>(event);
            QPainter painter(viewport());
            painter.setPen(Qt::blue);
            painter.drawText(mousePos, QString("Mouse at: [%1, %2]").arg(mousePos.x()).arg(mousePos.y()));
            return false; // Allow base class to handle the event
        }
    }
    return QScrollArea::eventFilter(watched, event); // Default handling for other events
}
void CustomScrollArea::paintEvent(QPaintEvent *event) {
    QScrollArea::paintEvent(event); // Ensure the base class paint logic runs
    qDebug("CustomScrollArea PaintEvent");
    QPainter painter(viewport());
    painter.setPen(Qt::blue);
    painter.drawText(mousePos, QString("Mouse at: [%1, %2]").arg(mousePos.x()).arg(mousePos.y()));
}

void CustomScrollArea::mouseMoveEvent(QMouseEvent *event) {
    qDebug("CustomScrollArea MouseMoveEvent");
    mousePos = event->pos();
    viewport()->update(); // Trigger the viewport's paint event
    update();
}
