/********************************************************************************
** Form generated from reading UI file 'tcpclient.ui'
**
** Created by: Qt User Interface Compiler version 5.12.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPCLIENT_H
#define UI_TCPCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TcpClient
{
public:
    QLabel *name_lab;
    QLabel *pwd_lab;
    QLineEdit *pwd_lin;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QPushButton *regist_pb;
    QSpacerItem *horizontalSpacer;
    QPushButton *login_pb;
    QSpacerItem *horizontalSpacer_2;
    QPushButton *cancel_pb;
    QLineEdit *name_lin;

    void setupUi(QWidget *TcpClient)
    {
        if (TcpClient->objectName().isEmpty())
            TcpClient->setObjectName(QString::fromUtf8("TcpClient"));
        TcpClient->resize(390, 300);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TcpClient->sizePolicy().hasHeightForWidth());
        TcpClient->setSizePolicy(sizePolicy);
        TcpClient->setMinimumSize(QSize(390, 300));
        TcpClient->setMaximumSize(QSize(390, 300));
        name_lab = new QLabel(TcpClient);
        name_lab->setObjectName(QString::fromUtf8("name_lab"));
        name_lab->setGeometry(QRect(50, 90, 80, 20));
        QFont font;
        font.setPointSize(12);
        name_lab->setFont(font);
        pwd_lab = new QLabel(TcpClient);
        pwd_lab->setObjectName(QString::fromUtf8("pwd_lab"));
        pwd_lab->setGeometry(QRect(50, 150, 80, 20));
        pwd_lab->setFont(font);
        pwd_lin = new QLineEdit(TcpClient);
        pwd_lin->setObjectName(QString::fromUtf8("pwd_lin"));
        pwd_lin->setGeometry(QRect(137, 142, 181, 41));
        pwd_lin->setEchoMode(QLineEdit::Password);
        layoutWidget = new QWidget(TcpClient);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(2, 200, 381, 71));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setContentsMargins(11, 11, 11, 11);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        regist_pb = new QPushButton(layoutWidget);
        regist_pb->setObjectName(QString::fromUtf8("regist_pb"));

        horizontalLayout->addWidget(regist_pb);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        login_pb = new QPushButton(layoutWidget);
        login_pb->setObjectName(QString::fromUtf8("login_pb"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(2);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(login_pb->sizePolicy().hasHeightForWidth());
        login_pb->setSizePolicy(sizePolicy1);
        login_pb->setMinimumSize(QSize(6, 0));

        horizontalLayout->addWidget(login_pb);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_2);

        cancel_pb = new QPushButton(layoutWidget);
        cancel_pb->setObjectName(QString::fromUtf8("cancel_pb"));

        horizontalLayout->addWidget(cancel_pb);

        name_lin = new QLineEdit(TcpClient);
        name_lin->setObjectName(QString::fromUtf8("name_lin"));
        name_lin->setGeometry(QRect(137, 80, 181, 41));
        QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(181);
        sizePolicy2.setVerticalStretch(41);
        sizePolicy2.setHeightForWidth(name_lin->sizePolicy().hasHeightForWidth());
        name_lin->setSizePolicy(sizePolicy2);

        retranslateUi(TcpClient);

        QMetaObject::connectSlotsByName(TcpClient);
    } // setupUi

    void retranslateUi(QWidget *TcpClient)
    {
        TcpClient->setWindowTitle(QApplication::translate("TcpClient", "TcpClient", nullptr));
        name_lab->setText(QApplication::translate("TcpClient", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        pwd_lab->setText(QApplication::translate("TcpClient", "\345\257\206  \347\240\201\357\274\232", nullptr));
        regist_pb->setText(QApplication::translate("TcpClient", "\346\263\250\345\206\214", nullptr));
        login_pb->setText(QApplication::translate("TcpClient", "\347\231\273\345\275\225", nullptr));
        cancel_pb->setText(QApplication::translate("TcpClient", "\346\263\250\351\224\200", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TcpClient: public Ui_TcpClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPCLIENT_H
