#include "tcpserver.h"
#include "ui_tcpserver.h"

#include "mytcpserver.h"
#include <QByteArray>//读取字节数据
#include <QDebug>
#include <QMessageBox>//数据信息打印
#include <QHostAddress>//ip地址转换

TcpServer::TcpServer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpServer)
{
    ui->setupUi(this);
    setWindowTitle("友享服务器");
    loadConfig();//加载配置文件
    MyTcpServer::getInstance().listen(QHostAddress(m_strIP),m_strPort);//事件监听
}

TcpServer::~TcpServer()
{
    delete ui;
}
//加载配置文件
void TcpServer::loadConfig()//
{
    QFile file(":./server.config");
    if(file.open(QIODevice::ReadOnly)){//只读打开
        QByteArray baData= file.readAll();//读取到字节数据
        QString strData=baData.toStdString().c_str();//字节数据转换成字符串数据 .c_str():转换成char  *型
        qDebug()<<strData;
        file.close();

        //"127.0.0.1\r\n8888"，将数据进行分割
        strData.replace("\r\n"," ");
        QStringList strList=strData.split(" ");//切分字符串，得到字符串列表

        //测试代码：
        for(int  i=0;i<strList.size();i++){
              qDebug()<<"-->"<<strList[i];
        }

        //可以拿到相应的端口和地址
        m_strIP=strList.at(0);
        m_strPort=strList.at(1).toUShort();//ip转换成短整型

        //测试代码
        qDebug()<<"ip"<<m_strIP<<"port"<<m_strPort;

    }else{
        QMessageBox::critical(this,"连接测试","打开配置失败");
    }

}
