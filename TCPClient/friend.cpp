#include "friend.h"

#include "protocol.h"
#include "tcpclient.h"
#include <QDebug>
#include "privatechate.h"
#include <QMessageBox>
Friend::Friend(QWidget *parent) : QWidget(parent)
{

    m_pShowMsgTE=new QTextEdit;//消息
    m_pFriendListWidget=new QListWidget;//好友列表
    m_pInputMsgLE=new QLineEdit ;//消息输入框


    m_pDelFriendPB=new QPushButton("删除好友");//删除好友
    m_pFlushFriendPB=new QPushButton("刷新好友列表");//刷新好友列表
    m_pShowOnlineUsrPB=new QPushButton("显示在线好友");//显示在线好友
    m_pSearchUsrPB=new QPushButton("查看好友");//查看好友
    m_pMsgSendPB=new QPushButton("群聊");;//发送消息：群聊
    m_pPrivateChatPB=new QPushButton("私聊");;//私聊

    m_pOnline=new  Online;//用户在线对象

    //添加按钮到右边
    QVBoxLayout * pRightPBVB=new QVBoxLayout;
    pRightPBVB->addWidget(m_pDelFriendPB);
    pRightPBVB->addWidget(m_pFlushFriendPB);
    pRightPBVB->addWidget(m_pShowOnlineUsrPB);
    pRightPBVB->addWidget(m_pSearchUsrPB);
    pRightPBVB->addWidget(m_pPrivateChatPB);
    //右边显示信息
    QHBoxLayout *pTopHBL=new QHBoxLayout;
    pTopHBL->addWidget(m_pShowMsgTE);
    pTopHBL->addWidget(m_pFriendListWidget);
    pTopHBL->addLayout(pRightPBVB);
    //信息输入框
    QHBoxLayout * pMsgHBL=new QHBoxLayout;
    pMsgHBL->addWidget(m_pInputMsgLE);
    pMsgHBL->addWidget(m_pMsgSendPB);
    //垂直布局
    QVBoxLayout * pMain=new QVBoxLayout;
    pMain->addLayout(pTopHBL);
    pMain->addLayout(pMsgHBL);
    //添加在线用户对象
    pMain->addWidget(m_pOnline);
    m_pOnline->hide();//默认不显示
    //添加所有部件
    setLayout(pMain);

    /*---------------关联按钮与信号---------------*/
    //关联点击显示好友列表信号
    connect(m_pShowOnlineUsrPB,SIGNAL(clicked(bool)),this,SLOT(showOnline()));

    //关联点击查找好友信号
    connect(m_pSearchUsrPB,SIGNAL(clicked(bool)),this,SLOT(searchUsr()));

    //关联刷新好友列表
    connect(m_pFlushFriendPB,SIGNAL(clicked(bool)),this,SLOT(flushFriend()));

    //关联删除好友
    connect(m_pDelFriendPB,SIGNAL(clicked(bool)),this,SLOT(delFriend()));

    //关联私聊
    connect(m_pPrivateChatPB,SIGNAL(clicked(bool)),this,SLOT(privateChat()));

    //关联群聊
    connect(m_pMsgSendPB,SIGNAL(clicked(bool)),this,SLOT(groupChat()));
}
//显示所有在线人名
void Friend::showAllOnlineUsr(PDU *pdu)
{
    if(pdu==NULL){
        return ;
    }
    m_pOnline->showUsr(pdu);//显示人名


}
//刷新好友列表
void Friend::updateFriendList(PDU *pdu)
{
    if(pdu==NULL){
        return ;
    }
    //循环提取服务器传过来的数据
    uint uiSize=pdu->uiMsgLen/32;//获取大小
    char caName[32]={'\0'};
    qDebug()<<"update friend list";
    for(uint i=0;i<uiSize;i++){
        mempcpy(caName,(char *)(pdu->caMsg)+i*32,32);//拷贝人名
        m_pFriendListWidget->addItem(caName);//添加进人名
        qDebug()<<"update friend list name"<<caName;
    }

}
//更新群聊消息
void Friend::updateGroupMsg(PDU *pdu)
{
    //消息显示拼接
    QString strMsg=QString("%1 says: %2").arg(pdu->caData).arg((char *)(pdu->caMsg));
    //显示在消息窗口上
    m_pShowMsgTE->append(strMsg);
}
//获得好友列表
QListWidget *Friend::getFriendLsit()
{
    return m_pFriendListWidget;
}
//添加在线用户
void Friend::showOnline()
{
    if(m_pOnline->isHidden()){
        m_pOnline->show();
        //发送显示在线用户请求
        PDU * pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_REQUEST;//所有在线请求
        //使用socket发送
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else {
    m_pOnline->hide();
    }
}
//查找用户
void Friend::searchUsr()
{
    //弹出搜索框：通过名字搜索：并把这个名字
    m_strSearchName=QInputDialog::getText(this,"查找用户","用户名");
    //test
    if(!m_strSearchName.isEmpty()){
        qDebug()<<"name"<<m_strSearchName;
        //打包发送给服务器
        PDU * pdu=mkPDU(0);
        //将名字封装套pdu中
        memcpy(pdu->caData,m_strSearchName.toStdString().c_str(),m_strSearchName.size());
        pdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USR_REQUEST;//查找用户请求
        //使用socket发送
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }

}
//刷新好友
void Friend::flushFriend()
{
    //每次刷新前都清空
    m_pFriendListWidget->clear();
    //将自己的用户名发送给服务器
    QString strName=TcpClient::getInstance().loginName();
    PDU *pdu=mkPDU(0);
    pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST;//刷新好友请求
    qDebug()<<"exit flush friend";
    memcpy(pdu->caData,strName.toStdString().c_str(),strName.size());
    //使用socket发送
    TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}
//删除好友
void Friend::delFriend()
{

    if(m_pFriendListWidget->currentItem()!=NULL){
        //从列表中删除好友
        QString strFriendName=m_pFriendListWidget->currentItem()->text();
        qDebug()<<"del friend name:"<<strFriendName;
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType= ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST;//删除好友
        QString strSelfName=TcpClient::getInstance().loginName();//获取自己的登录名字
        //将自己的名字和朋友的名字都发送给服务端
        mempcpy(pdu->caData,strSelfName.toStdString().c_str(),strSelfName.size());
        mempcpy(pdu->caData+32,strFriendName.toStdString().c_str(),strFriendName.size());
        //发送
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }


}
//私聊
void Friend::privateChat()
{
    //设置需要好友名字
    if(m_pFriendListWidget->currentItem()!=NULL){
         QString strChatName=m_pFriendListWidget->currentItem()->text();//获取私聊名
         PrivateChate::getInstance().setChatName(strChatName);//传递
         //如果隐藏，则进行显示
         if( PrivateChate::getInstance().isHidden()){
             PrivateChate::getInstance().show();
         }
    }else{
        QMessageBox::warning(this,"私聊","请选择用户");
    }


}
//群聊
void Friend::groupChat()
{
    //获取群聊消息输入框的数据
    m_pInputMsgLE->setInputMethodHints(Qt::ImhPreferUppercase);//设置字符集
    QString strMsg=m_pInputMsgLE->text();
    //获取后就清除
//    m_pInputMsgLE->clear();//先不清除
    if(!strMsg.isEmpty()){
       // PDU *pdu=mkPDU(strMsg.size()+1);
        PDU *pdu = mkPDU(strMsg.toUtf8().size()+1);//中文乱码问题解决
        pdu->uiMsgType=ENUM_MSG_TYPE_GROUP_CHAT_REQUEST;//群聊信息
        QString strName=TcpClient::getInstance().loginName();//获取登录名
        strncpy(pdu->caData,strName.toStdString().c_str(),strName.size());
        //数据部分
        memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.toUtf8().size());
        //发送
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;

    }else{
        QMessageBox::warning(this,"群聊","群聊消息为空");
    }
}
