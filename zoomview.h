#ifndef ZOOMVIEW_H
#define ZOOMVIEW_H

#include <QGraphicsView>
#include <QVariantAnimation>
#include <QRectF>
#include <QPointF>
#include <QPoint>

// QGraphicsView com zoom (Ctrl+roda, botões), pan com o botão do meio e
// transições animadas para "ver tudo" ou focar em colunas (fases).
class ZoomView : public QGraphicsView
{
    Q_OBJECT
public:
    explicit ZoomView(QGraphicsScene *scene, QWidget *parent = nullptr);

    qreal zoom() const;

public slots:
    // enquadra a cena inteira; continua enquadrando se a janela mudar de tamanho
    void fitAll(bool animated = true);
    // foca num intervalo de colunas: ocupa a largura (até FOCUS_MAX_SCALE),
    // alinhado ao topo do retângulo
    void focusColumns(const QRectF &sceneRect, bool animated = true);
    void zoomIn();
    void zoomOut();

signals:
    void zoomChanged(qreal scale);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    void animateTo(qreal scale, const QPointF &center, bool animated);
    void applyView(qreal scale, const QPointF &center);
    void scaleBy(qreal factor);
    qreal fitScale() const;
    qreal minScale() const;
    QPointF visibleCenter() const;

    QVariantAnimation *anim;
    qreal fromScale = 1, toScale = 1;
    QPointF fromCenter, toCenter;
    bool autoFit = true;
    bool panning = false;
    QPoint panLast;
};

#endif // ZOOMVIEW_H
