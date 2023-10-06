#ifndef MYTCPSERVER_H
#define MYTCPSERVER_H

#include <QTcpServer>
#include <QList>//将socket都保存到链表中
#include  "mytcpsocket.h"
#include "mytcpsocket.h"
class MyTcpServer : public QTcpServer
{
    //Q_OBJECT才能支持信号槽
    Q_OBJECT
public:
    MyTcpServer();
    static MyTcpServer &getInstance();
    //客户端连接后就会调用该函数
    void incomingConnection(qintptr socketDescriptor);
    //转发MyTcpSocket，因为所有的socket都保存在其中
    void resend(const char * prename,PDU * pdu);

public slots:
    void deleteSocket(MyTcpSocket * mysocket);//当发送下线信号时，该槽函数就会触发
private:
    QList<MyTcpSocket * > m_tcpSocketList;//保存socket到链表中

};

#endif // MYTCPSERVER_H
