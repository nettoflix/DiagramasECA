#include "fasetitle.h"

FaseTitle::FaseTitle(QWidget *parent, QString text) : QWidget(parent)
{
    QLabel* label = new QLabel(this);
    QFont f( "Arial", 20);
    label->setFont( f);

    QVBoxLayout* layout = new QVBoxLayout(this);  // Create a vertical layout
    layout->setContentsMargins(0, 0, 0, 0);  // Remove margins

    label->setText(text);
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    this->setLayout(layout);

    this->setFixedSize(300,200);


}
