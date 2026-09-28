#include "widget.h"
#include "diagram.h"
#include <QApplication>
#include <QTimer>
#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include "zoomview.h"
#include <QGridLayout>
#include <QStyle>
#include <cstdio>
#include <QPixmap>
#include <QProcessEnvironment>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();
    QTimer::singleShot(2500, [&]() {
        // o container fica dentro de um QGraphicsProxyWidget da ZoomView
        ZoomView* view = w.findChild<ZoomView*>();
        QWidget* container = nullptr;
        for (QGraphicsItem* it : view->scene()->items())
            if (QGraphicsProxyWidget* px = qgraphicsitem_cast<QGraphicsProxyWidget*>(it))
                container = px->widget();
        QList<Diagram*> ds = container->findChildren<Diagram*>();
        QGridLayout* gl = qobject_cast<QGridLayout*>(container->layout());
        printf("STYLE %s\n", qPrintable(a.style()->objectName()));
        printf("WINDOW %d %d %d %d\n", w.geometry().x(), w.geometry().y(), w.width(), w.height());
        printf("VIEWPORT %d %d zoom %.3f\n", view->viewport()->width(), view->viewport()->height(), view->zoom());
        printf("CONTAINER %d %d %d %d\n", container->x(), container->y(), container->width(), container->height());
        printf("CONTAINER_MIN %d %d\n", container->minimumSizeHint().width(), container->minimumSizeHint().height());
        int l,t,r,b; gl->getContentsMargins(&l,&t,&r,&b);
        printf("GRID margins %d %d %d %d hsp %d vsp %d rows %d cols %d\n", l,t,r,b, gl->horizontalSpacing(), gl->verticalSpacing(), gl->rowCount(), gl->columnCount());
        for (int rr=0; rr<gl->rowCount(); rr++) { QRect c = gl->cellRect(rr,0); printf("ROW %d y %d h %d\n", rr, c.y(), c.height()); }
        for (int cc=0; cc<gl->columnCount(); cc++) { QRect c = gl->cellRect(0,cc); printf("COL %d x %d w %d\n", cc, c.x(), c.width()); }
        for (Diagram* d : ds) {
            int idx = gl->indexOf(d); int row=-1,col=-1,rs,cs;
            if (idx>=0) gl->getItemPosition(idx,&row,&col,&rs,&cs);
            printf("DIAG\t%s\t%d\t%d\t%d\t%d\t%d\t%d\n", d->name.toUtf8().constData(), row, col, d->x(), d->y(), d->width(), d->height());
        }
        QString out = QProcessEnvironment::systemEnvironment().value("PROBE_PNG");
        // grab() do container: renderização 1:1, independente do zoom da vista
        if (!out.isEmpty()) { container->grab().save(out); printf("SAVED %s\n", qPrintable(out)); }
        fflush(stdout);
        a.quit();
    });
    return a.exec();
}
