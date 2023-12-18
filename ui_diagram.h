/********************************************************************************
** Form generated from reading UI file 'diagram.ui'
**
** Created by: Qt User Interface Compiler version 6.6.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIAGRAM_H
#define UI_DIAGRAM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Diagram
{
public:
    QWidget *verticalLayoutWidget;
    QVBoxLayout *verticalLayout;
    QLabel *label;

    void setupUi(QWidget *Diagram)
    {
        if (Diagram->objectName().isEmpty())
            Diagram->setObjectName("Diagram");
        Diagram->resize(323, 300);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Diagram->sizePolicy().hasHeightForWidth());
        Diagram->setSizePolicy(sizePolicy);
        Diagram->setMinimumSize(QSize(300, 300));
        Diagram->setBaseSize(QSize(300, 300));
        verticalLayoutWidget = new QWidget(Diagram);
        verticalLayoutWidget->setObjectName("verticalLayoutWidget");
        verticalLayoutWidget->setGeometry(QRect(30, 50, 160, 80));
        verticalLayout = new QVBoxLayout(verticalLayoutWidget);
        verticalLayout->setObjectName("verticalLayout");
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(verticalLayoutWidget);
        label->setObjectName("label");
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);
        label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label);


        retranslateUi(Diagram);

        QMetaObject::connectSlotsByName(Diagram);
    } // setupUi

    void retranslateUi(QWidget *Diagram)
    {
        Diagram->setWindowTitle(QCoreApplication::translate("Diagram", "Form", nullptr));
        label->setText(QCoreApplication::translate("Diagram", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Diagram: public Ui_Diagram {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIAGRAM_H
