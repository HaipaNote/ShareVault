/********************************************************************************
** Form generated from reading UI file 'privatechate.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_PRIVATECHATE_H
#define UI_PRIVATECHATE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PrivateChate
{
public:
    QVBoxLayout *verticalLayout_2;
    QVBoxLayout *verticalLayout;
    QTextEdit *showMsg_te;
    QHBoxLayout *horizontalLayout;
    QLineEdit *inputMsg_le;
    QPushButton *sendMsg_pb;

    void setupUi(QWidget *PrivateChate)
    {
        if (PrivateChate->objectName().isEmpty())
            PrivateChate->setObjectName(QString::fromUtf8("PrivateChate"));
        PrivateChate->resize(558, 408);
        verticalLayout_2 = new QVBoxLayout(PrivateChate);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        showMsg_te = new QTextEdit(PrivateChate);
        showMsg_te->setObjectName(QString::fromUtf8("showMsg_te"));
        QFont font;
        font.setPointSize(20);
        showMsg_te->setFont(font);
        showMsg_te->setInputMethodHints(Qt::ImhMultiLine|Qt::ImhPreferUppercase);

        verticalLayout->addWidget(showMsg_te);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        inputMsg_le = new QLineEdit(PrivateChate);
        inputMsg_le->setObjectName(QString::fromUtf8("inputMsg_le"));
        inputMsg_le->setFont(font);

        horizontalLayout->addWidget(inputMsg_le);

        sendMsg_pb = new QPushButton(PrivateChate);
        sendMsg_pb->setObjectName(QString::fromUtf8("sendMsg_pb"));
        sendMsg_pb->setFont(font);

        horizontalLayout->addWidget(sendMsg_pb);


        verticalLayout->addLayout(horizontalLayout);


        verticalLayout_2->addLayout(verticalLayout);


        retranslateUi(PrivateChate);

        QMetaObject::connectSlotsByName(PrivateChate);
    } // setupUi

    void retranslateUi(QWidget *PrivateChate)
    {
        PrivateChate->setWindowTitle(QApplication::translate("PrivateChate", "Form", nullptr));
        sendMsg_pb->setText(QApplication::translate("PrivateChate", "\345\217\221\351\200\201", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PrivateChate: public Ui_PrivateChate {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_PRIVATECHATE_H
