#ifndef MYTCPSOCKET_H
#define MYTCPSOCKET_H


#include <QTcpSocket>
#include "protocol.h"
#include "opedb.h"
#include <QFile>
#include <QTimer>

class MyTcpSocket : public QTcpSocket
{
   Q_OBJECT//同理
signals:
    void offline(MyTcpSocket * mysocket);//下线的信号，通过传递地址得到socket
public:
    MyTcpSocket();
    QString getName();//用于获取私有的m_strName
    void copyDir(QString strSrcDir,QString strDestDir);//拷贝文件夹
public slots:
    void recvMsg();//用于接收消息
    void clientOfflinet();//用于处理客户端下线信号
    void sendFileToClient();//发送数据给客户端
private:
    QString m_strName;//储存用户名字
    QFile m_file;//文件名字
    qint64 m_iTotal;//文件大小
    qint64 m_iRecved;//已经接受的文件大小
    bool m_bUpload;//处于上传文件状态还是其他状态
    QTimer *m_pTimer;//发送数据定时器
};

#endif // MYTCPSOCKET_H
