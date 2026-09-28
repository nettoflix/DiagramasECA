/********************************************************************************
** Form generated from reading UI file 'diagram.ui'
**
** Created by: Qt User Interface Compiler version 5.9.6
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIAGRAM_H
#define UI_DIAGRAM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Diagram
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QLabel *label;

    void setupUi(QWidget *Diagram)
    {
        if (Diagram->objectName().isEmpty())
            Diagram->setObjectName(QStringLiteral("Diagram"));
        Diagram->resize(150, 300);
        QSizePolicy sizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(Diagram->sizePolicy().hasHeightForWidth());
        Diagram->setSizePolicy(sizePolicy);
        Diagram->setMinimumSize(QSize(120, 130));
        Diagram->setMaximumSize(QSize(150, 400));
        Diagram->setBaseSize(QSize(120, 500));
        Diagram->setStyleSheet(QLatin1String("QLabel{\n"
"border: none;\n"
"background-color: pink;\n"
"}"));
        verticalLayout_2 = new QVBoxLayout(Diagram);
        verticalLayout_2->setObjectName(QStringLiteral("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        label = new QLabel(Diagram);
        label->setObjectName(QStringLiteral("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Maximum);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);
        label->setMaximumSize(QSize(300, 400));
        label->setBaseSize(QSize(120, 220));
        QFont font;
        font.setPointSize(13);
        font.setBold(true);
        label->setFont(font);
        label->setAlignment(Qt::AlignCenter);

        verticalLayout->addWidget(label);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(Diagram);

        QMetaObject::connectSlotsByName(Diagram);
    } // setupUi

    void retranslateUi(QWidget *Diagram)
    {
        Diagram->setWindowTitle(QApplication::translate("Diagram", "Form", Q_NULLPTR));
        label->setText(QApplication::translate("Diagram", "TextLabel", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Diagram: public Ui_Diagram {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIAGRAM_H
