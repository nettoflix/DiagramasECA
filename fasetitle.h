#ifndef FASETITLE_H
#define FASETITLE_H

#include <QWidget>
#include <QString>
#include <QLabel>
#include <QVBoxLayout>
class FaseTitle : public QWidget
{
    Q_OBJECT
public:
    explicit FaseTitle(QWidget *parent = nullptr, QString text="");

signals:

public slots:
};

#endif // FASETITLE_H
