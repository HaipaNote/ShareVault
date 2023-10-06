#include "mytcpserver.h"

#include <QDebug>
MyTcpServer::MyTcpServer()
{

}
//客户端连接后就会调用该函数
MyTcpServer &MyTcpServer::getInstance()
{
    static MyTcpServer instance;
    return instance;
}
//连接成功后，返回信息
void MyTcpServer::incomingConnection(qintptr socketDescriptor)
{
    qDebug()<<"new client connected";
    MyTcpSocket * pTcpSocket=new MyTcpSocket;//创建socket对象
    pTcpSocket->setSocketDescriptor(socketDescriptor);//保存后就可以完成通讯
    m_tcpSocketList.append(pTcpSocket);//不断向链表尾部添加socket

    //关联下线
    connect(pTcpSocket,SIGNAL(offline(MyTcpSocket *)),this,SLOT(deleteSocket(MyTcpSocket *)));

}
//在mytcpsocket转发
void MyTcpServer::resend(const char *prename, PDU *pdu)
{
    if(prename==NULL || pdu==NULL){
        return ;
    }
    QString strName=prename;
    //遍历列表：循环遍历发送
    for(int i=0;i<m_tcpSocketList.size();i++){
        if(strName== m_tcpSocketList.at(i)->getName()){
            m_tcpSocketList.at(i)->write((char * )pdu,pdu->uiPDULen);//发送
            break;
        }

    }

}
//处理下线信号的槽
void MyTcpServer::deleteSocket(MyTcpSocket *mysocket)
{
    //不断遍历socket链表，查找到就可以进行删除
    QList<MyTcpSocket * >::iterator it=m_tcpSocketList.begin();
    for (;it!=m_tcpSocketList.end();it++) {
        if(mysocket==*it){
//            delete  *it;//先删除内容
//            *it=NULL;//将指向这个区域的指针置空
            m_tcpSocketList.erase(it);//释放指针
            break;
        }
    }
    //打印结果
    for(int i=0;i<m_tcpSocketList.size();i++){
        qDebug()<<"name :"<<m_tcpSocketList.at(i)->getName();
    }


}
