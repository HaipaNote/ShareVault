#ifndef FRIEND_H
#define FRIEND_H

#include <QWidget>

#include <QTextEdit>
#include <QListWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QInputDialog>//用于接收搜索输入
#include "online.h"

class Friend : public QWidget
{
    Q_OBJECT
public:
    explicit Friend(QWidget *parent = nullptr);
    void showAllOnlineUsr(PDU *pdu);//显示所有在线人名
    QString m_strSearchName;//存储临时的名字
    void updateFriendList(PDU *pdu);//刷新好友列表
    void updateGroupMsg(PDU *pdu);//更新群聊消息
    QListWidget * getFriendLsit();//获得好友列表
signals:

public slots:

    void showOnline();//显示在线用户
    void searchUsr();//查找用户
    void flushFriend();//刷新好友
    void delFriend();//删除好友
    void privateChat();//私聊
    void groupChat();//群聊

private:
    QTextEdit * m_pShowMsgTE;//消息
    QListWidget * m_pFriendListWidget;//好友列表
    QLineEdit * m_pInputMsgLE;//消息输入框
    QPushButton * m_pDelFriendPB;//删除好友
    QPushButton * m_pFlushFriendPB;//刷新好友列表
    QPushButton * m_pShowOnlineUsrPB;//显示在线好友
    QPushButton * m_pSearchUsrPB;//查看好友
    QPushButton * m_pMsgSendPB;//发送消息:群聊
    QPushButton * m_pPrivateChatPB;//私聊

    Online * m_pOnline;//用户在线对象


};

#endif // FRIEND_H
