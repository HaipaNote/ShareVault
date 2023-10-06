#include "opewidget.h"
#include "tcpclient.h"
#include <QLabel>

#define WELCOMTO_USER "welcome "

OpeWidget::OpeWidget(QWidget *parent) : QWidget(parent)
{
    this->setWindowTitle("友享网盘");


    m_pListW=new QListWidget(this);
    m_pListW->setStyleSheet(
                "QListWidget {"
                "    background-color: rgba(255, 255, 255, 150); /* 半透明白色背景 */"
                "    border: 2px solid #ccc; /* 边框为灰色 */"
                "    border-radius: 8px; /* 圆角半径为10像素 */"
                "}");


     //左边框:好友和图书列表
    m_pListW->addItem("我的好友");
    m_pListW->addItem("共享图书");

    //添加用户名显示
    QString welcome_User=WELCOMTO_USER;
    QString m_pUserName=TcpClient::getInstance().loginName().toUtf8();
    welcome_User.append(m_pUserName);

    QLabel* welcome_Label = new QLabel(welcome_User.toUtf8());




    //每次只显示一个
    m_pFriend=new Friend;//好友对象
    m_pBook=new Book;//图书对象
    m_pSw=new QStackedWidget;//堆栈窗口

    m_pSw->addWidget(m_pFriend);
    m_pSw->addWidget(m_pBook);

    QVBoxLayout * left=new QVBoxLayout;
    left->addWidget(welcome_Label);
    left->addWidget(m_pListW);


    QHBoxLayout * pMain=new QHBoxLayout;

    pMain->addLayout(left);
    pMain->addWidget(m_pSw);
    setLayout(pMain);
    //添加信号槽关联
    connect(m_pListW,SIGNAL(currentRowChanged(int)),m_pSw, SLOT(setCurrentIndex(int)));

}
//单利模式实现
OpeWidget &OpeWidget::getInstance()
{
    static OpeWidget instant;
    return instant;
}
//将好友的私有属性返回
Friend *OpeWidget::getFriend()
{
    return m_pFriend;
}
//获得book界面属性
Book *OpeWidget::getBook()
{
    return m_pBook;
}
