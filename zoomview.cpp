#include "zoomview.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QScrollBar>
#include <QEasingCurve>
#include <QtMath>

namespace {
const qreal MAX_SCALE = 3.0;        // zoom máximo (300%)
const qreal FOCUS_MAX_SCALE = 1.5;  // zoom máximo ao focar fases (uma fase sozinha não fica gigante)
const qreal MARGIN = 20.0;          // folga em px de cena ao enquadrar
}

ZoomView::ZoomView(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
{
    setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorViewCenter);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setDragMode(QGraphicsView::NoDrag); // botão esquerdo continua clicando nos diagramas
    setBackgroundBrush(palette().window());

    anim = new QVariantAnimation(this);
    anim->setDuration(250);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    connect(anim, &QVariantAnimation::valueChanged, this, [this](const QVariant &v) {
        qreal t = v.toReal();
        // interpola a escala geometricamente: o zoom "parece" linear
        qreal s = fromScale * qPow(toScale / fromScale, t);
        QPointF c = fromCenter + (toCenter - fromCenter) * t;
        applyView(s, c);
    });
}

qreal ZoomView::zoom() const
{
    return transform().m11();
}

QPointF ZoomView::visibleCenter() const
{
    return mapToScene(viewport()->rect().center());
}

qreal ZoomView::fitScale() const
{
    if (!scene())
        return 1.0;
    QRectF r = scene()->sceneRect();
    if (r.isEmpty())
        return 1.0;
    return qMin(viewport()->width() / (r.width() + 2 * MARGIN),
                viewport()->height() / (r.height() + 2 * MARGIN));
}

qreal ZoomView::minScale() const
{
    return qMin(fitScale() * 0.9, 1.0);
}

void ZoomView::applyView(qreal scale, const QPointF &center)
{
    setTransform(QTransform::fromScale(scale, scale));
    centerOn(center);
    emit zoomChanged(scale);
}

void ZoomView::animateTo(qreal scale, const QPointF &center, bool animated)
{
    anim->stop();
    scale = qBound(minScale(), scale, MAX_SCALE);
    if (!animated) {
        applyView(scale, center);
        return;
    }
    fromScale = zoom();
    fromCenter = visibleCenter();
    toScale = scale;
    toCenter = center;
    anim->start();
}

void ZoomView::fitAll(bool animated)
{
    if (!scene())
        return;
    autoFit = true;
    animateTo(fitScale(), scene()->sceneRect().center(), animated);
}

void ZoomView::focusColumns(const QRectF &rect, bool animated)
{
    autoFit = false;
    qreal s = qMin(viewport()->width() / (rect.width() + 2 * MARGIN), FOCUS_MAX_SCALE);
    s = qMax(s, fitScale()); // nunca menor que "ver tudo"
    // centro horizontal nas colunas; topo da vista no topo do retângulo (títulos das fases)
    qreal halfH = viewport()->height() / s / 2;
    QPointF c(rect.center().x(), rect.top() - MARGIN + halfH);
    animateTo(s, c, animated);
}

void ZoomView::scaleBy(qreal factor)
{
    anim->stop();
    autoFit = false;
    qreal s = qBound(minScale(), zoom() * factor, MAX_SCALE);
    factor = s / zoom();
    if (qFuzzyCompare(factor, 1.0))
        return;
    scale(factor, factor);
    emit zoomChanged(zoom());
}

void ZoomView::zoomIn()
{
    ViewportAnchor old = transformationAnchor();
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    scaleBy(1.25);
    setTransformationAnchor(old);
}

void ZoomView::zoomOut()
{
    ViewportAnchor old = transformationAnchor();
    setTransformationAnchor(QGraphicsView::AnchorViewCenter);
    scaleBy(1 / 1.25);
    setTransformationAnchor(old);
}

void ZoomView::wheelEvent(QWheelEvent *event)
{
    if (event->modifiers() & Qt::ControlModifier) {
        // ancora explícita: o ponto da cena sob o cursor continua sob o cursor
        QPoint vp = event->pos();
        QPointF before = mapToScene(vp);
        ViewportAnchor old = transformationAnchor();
        setTransformationAnchor(QGraphicsView::NoAnchor);
        scaleBy(qPow(1.15, event->angleDelta().y() / 120.0));
        setTransformationAnchor(old);
        QPointF delta = mapFromScene(before) - QPointF(vp);
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + qRound(delta.x()));
        verticalScrollBar()->setValue(verticalScrollBar()->value() + qRound(delta.y()));
        event->accept();
        return;
    }
    if (event->modifiers() & Qt::ShiftModifier) {
        QScrollBar *h = horizontalScrollBar();
        h->setValue(h->value() - event->angleDelta().y());
        event->accept();
        return;
    }
    QGraphicsView::wheelEvent(event);
}

void ZoomView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton) {
        anim->stop();
        autoFit = false;
        panning = true;
        panLast = event->pos();
        viewport()->setCursor(Qt::ClosedHandCursor);
        event->accept();
        return;
    }
    QGraphicsView::mousePressEvent(event);
}

void ZoomView::mouseMoveEvent(QMouseEvent *event)
{
    if (panning) {
        QPoint d = event->pos() - panLast;
        panLast = event->pos();
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - d.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - d.y());
        event->accept();
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void ZoomView::mouseReleaseEvent(QMouseEvent *event)
{
    if (panning && event->button() == Qt::MiddleButton) {
        panning = false;
        viewport()->unsetCursor();
        event->accept();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void ZoomView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    if (autoFit)
        fitAll(false);           // ex.: a janela foi maximizada depois de aberta
    else if (zoom() < minScale())
        applyView(minScale(), visibleCenter());
}
