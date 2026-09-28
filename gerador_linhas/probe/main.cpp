#include "widget.h"
#include "diagram.h"
#include <QApplication>
#include <QTimer>
#include <QScrollArea>
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
        QList<Diagram*> ds = w.findChildren<Diagram*>();
        QWidget* container = ds.first()->parentWidget();
        QScrollArea* sa = w.findChild<QScrollArea*>();
        QGridLayout* gl = qobject_cast<QGridLayout*>(container->layout());
        printf("STYLE %s\n", qPrintable(a.style()->objectName()));
        printf("WINDOW %d %d %d %d\n", w.geometry().x(), w.geometry().y(), w.width(), w.height());
        printf("VIEWPORT %d %d\n", sa->viewport()->width(), sa->viewport()->height());
        printf("CONTAINER %d %d %d %d\n", container->x(), container->y(), container->width(), container->height());
        printf("SCROLLAREA_IN_WIDGET %d %d frame %d\n", sa->x(), sa->y(), sa->frameWidth());
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
        if (!out.isEmpty()) { container->grab().save(out); printf("SAVED %s\n", qPrintable(out)); }
        fflush(stdout);
        a.quit();
    });
    return a.exec();
}
