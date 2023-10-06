#include "tcpclient.h"
#include "ui_tcpclient.h"

#include <QByteArray>//读取字节数据
#include <QDebug>
#include <QMessageBox>//数据信息打印
#include <QHostAddress>//ip地址转换
#include "protocol.h"
#include "privatechate.h"

TcpClient::TcpClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TcpClient)
{
    ui->setupUi(this);

    this->setWindowTitle("友享网盘");

    loadConfig();

    //连接服务器
    connect(&m_tcpSocket,SIGNAL(connected()),this,SLOT(showConnect()));

    //接收消息
    connect(&m_tcpSocket,SIGNAL(readyRead()),this,SLOT(recvMsg()));

    //通过socket连接服务器
    m_tcpSocket.connectToHost(QHostAddress(m_strIP),m_strPort);
}

TcpClient::~TcpClient()
{
    delete ui;
}
//加载配置文件
void TcpClient::loadConfig()
{
    QFile file(":./client.config");
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
        QMessageBox::critical(this,"加载配置","打开连接失败");
    }

}
//实现单例模式
TcpClient &TcpClient::getInstance()
{
    static TcpClient instance;
    return instance;
}
//返回socket
QTcpSocket &TcpClient::getTcpSocket()
{
    return m_tcpSocket;
}
//返回私有登录名
QString TcpClient::loginName()
{
    return m_strLoginName;

}
//返回当前路径信息
QString TcpClient::curPath()
{
    return m_strCurPath;
}
//设置当前路径
void TcpClient::setCurPath(QString strCurPath)
{
    m_strCurPath=strCurPath;
}
//显示连接状态
void TcpClient::showConnect()
{

    QMessageBox::information(this,"连接服务器","连接成功");
}
//接收服务器返回数据
void TcpClient::recvMsg()
{

    if(!OpeWidget::getInstance().getBook()->getDownloadStatus()){
        //test
        qDebug()<<m_tcpSocket.bytesAvailable();//当前可读的数据大小
        uint uiPDULen=0;
        m_tcpSocket.read((char *)&uiPDULen,sizeof(uint));//先收4字节的长度，这四个字节就包含了整个数据包大小
        uint uiMsgLen=uiPDULen-sizeof(PDU);//实际消息长度=总长（协议大小）-固定长度（就是除动态数组部分）
        PDU *pdu=mkPDU(uiMsgLen);//接受剩余数据长度
        m_tcpSocket.read((char *)pdu+sizeof(uint),uiPDULen-sizeof(uint));//往后偏移
        qDebug()<<pdu->uiMsgType<<(char *)(pdu->caMsg);//test

        qDebug()<<"status:"<<pdu->caData<<"msgType:"<<pdu->uiMsgType;//test

        //首先判断一下消息类型
        switch(pdu->uiMsgType){
        //将数据添加到数据库
        case ENUM_MSG_TYPE_REGIST_RESPOND://注册类型的请求
        {
            if(0==strcmp(pdu->caData,REGIST_OK)){
                QMessageBox::information(this,"注册",REGIST_OK);
            }else if(0==strcmp(pdu->caData,REGIST_FAILED)){
                QMessageBox::warning(this,"注册",REGIST_FAILED);
            }
            break;
        }

        case ENUM_MSG_TYPE_LOGIN_RESPOND://登录类型请求
        {
            if(0==strcmp(pdu->caData,LOGIN_OK)){

                m_strCurPath=QString("./%1").arg(m_strLoginName);//记录登录时的一个文件名

                QMessageBox::information(this,"登录",LOGIN_OK);
                //通过调用静态函数，显示该窗口
                OpeWidget::getInstance().show();
                //同时关闭登录窗口
                this->hide();



            }else if(0==strcmp(pdu->caData,LOGIN_FAILED)){
                QMessageBox::warning(this,"登录",LOGIN_FAILED);
            }
            break;
        }
        case ENUM_MSG_TYPE_ALL_ONLINE_RESPOND://查询所有在线人名
        {
            //循环将名字提取出来，然后放在列表中显示
            OpeWidget::getInstance().getFriend()->showAllOnlineUsr(pdu);

            break;
        }
        case ENUM_MSG_TYPE_SEARCH_USR_RESPOND://查找制定人
        {
            if(strcmp(SEARCH_USR_NO,pdu->caData)==0){//查找人不存在
                QMessageBox::information(this,"查找用户",QString("%1 :not exit").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));//获取临时保存的name
            }else if(strcmp(SEARCH_USR_ONLINE,pdu->caData)==0){
                QMessageBox::information(this,"查找用户",QString("%1 :people online").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));//获取临时保存的name
            }else if(strcmp(SEARCH_USR_OFFLINE,pdu->caData)==0){
                QMessageBox::information(this,"查找用户",QString("%1 :people offonline").arg(OpeWidget::getInstance().getFriend()->m_strSearchName));//获取临时保存的name
            }
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_RESPOND://添加好友回复
        {
            //打印信息
            QMessageBox::information(this,"添加好友",pdu->caData);

            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST:{//添加好友请求

            //先拿到对方名字
            char caName[32]={'\0'};
            strncpy(caName,pdu->caData+32,32);
            int ret=QMessageBox::information(this,"添加好友",QString("%1 want to add friend ?").arg(caName),QMessageBox::Yes,QMessageBox::No);
            PDU *respdu=mkPDU(0);
            memcpy(respdu->caData,pdu->caData,64);//拷贝的是是自己的名字
            if(ret==QMessageBox::Yes){
                respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_AGREE;//同意添加
            }else  {
                respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REFUSE;//不同意添加
            }
            //发送
            m_tcpSocket.write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_AGREE://同意添加
        {
            char caPerName[32] = {'\0'};
            memcpy(caPerName, pdu->caData, 32);
            QMessageBox::information(this, "添加好友", QString("添加%1好友成功").arg(caPerName));
            break;
        }
        case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE://不同意添加
        {
            char caPerName[32] = {'\0'};
            memcpy(caPerName, pdu->caData, 32);
            QMessageBox::information(this, "添加好友", QString("添加%1好友失败").arg(caPerName));
            break;
        }

        case ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND://刷新好友相应
        {
            OpeWidget::getInstance().getFriend()->updateFriendList(pdu);
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST://删除好友请求
        {
            //获取对方名字
            char caName[32]={'\0'};
            memcpy(caName,pdu->caData+32,32);
            QMessageBox::information(this,"删除好友",QString("%1 delete you as friend").arg(caName));
            break;
        }
        case ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND://删除好友服务器响应
        {
            QMessageBox::information(this,"删除好友",DEL_FRIEND_OK);
            break;
        }
        case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST://私聊请求
        {
            //如果隐藏则显示该框
            if( PrivateChate::getInstance().isHidden()){
                PrivateChate::getInstance().show();
            }
            //获取caSendName
            char caSendName[32]={'\0'};
            memcpy(caSendName,pdu->caData,32);
            QString strSendName=caSendName;
            PrivateChate::getInstance().setChatName(strSendName);//设置发送者名字

            //将聊天信息展示在窗口中
            PrivateChate::getInstance().updateMsg(pdu);
            break;
        }
        case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST://群聊请求
        {
            OpeWidget::getInstance().getFriend()->updateGroupMsg(pdu);//调用窗口
            break;
        }
        case ENUM_MSG_TYPE_CREATE_DIR_RESPOND://创建文件夹消息
        {
            QMessageBox::information(this,"创建文件夹",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_FLUSH_FILE_RESPOND://刷新文件回复
        {
            OpeWidget::getInstance().getBook()->updateFileList(pdu);
            QString strEnterDir=OpeWidget::getInstance().getBook()->enterDir();
            if(!strEnterDir.isEmpty()){
                m_strCurPath=m_strCurPath+"/"+strEnterDir;
                qDebug()<<"enter dir:"<<m_strCurPath;
            }
            break;
        }
        case ENUM_MSG_TYPE_DEL_DIR_RESPOND://删除文件
        {
            QMessageBox::information(this,"删除文件夹",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_RENAME_FILE_RESPOND://重命名
        {
            QMessageBox::information(this,"重命名文件夹",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_ENTER_DIR_RESPOND://进入文件夹
        {
            OpeWidget::getInstance().getBook()->clearEnterDir();
            QMessageBox::information(this,"进入文件夹",pdu->caData);

            break;
        }
        case ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND://上传文件
        {

            QMessageBox::information(this,"上传文件",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_DEL_FILE_RESPOND://删除文件
        {

            QMessageBox::information(this,"删除文件",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND://下载文件
        {
            qDebug()<<"文件名和大小："<<pdu->caData;
            //提取文件名和大小
            char caFileName[32]={'\0'};
            sscanf(pdu->caData,"%s %lld",caFileName,&(OpeWidget::getInstance().getBook()->m_iTotal));
            qDebug()<<"文件名："<<caFileName<<"大小："<<OpeWidget::getInstance().getBook()->m_iTotal;//test

            if(strlen(caFileName)>0 && OpeWidget::getInstance().getBook()->m_iTotal>0){
                OpeWidget::getInstance().getBook()->setDownLoadStatus(true);//处于下载状态
                //打开文件
                m_file.setFileName(OpeWidget::getInstance().getBook()->getSaveFilePath());
                if(!m_file.open(QIODevice::WriteOnly)){
                    QMessageBox::warning(this,"下载文件","获得路径失败");
                }

            }
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_RESPOND://共享文件
        {
            QMessageBox::information(this,"共享文件",pdu->caData);
            break;
        }
        case ENUM_MSG_TYPE_SHARE_FILE_NOTE://共享文件通知
        {
            //提取文件名
            char * pPath=new char[pdu->uiMsgLen];
            memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
            char * pos=strrchr(pPath,'/');//从后面找一个字符
            if(NULL!=pos){
                pos++;
                QString strNote=QString("%1 share file %2\n Do you accept?").arg(pdu->caData).arg(pos).toUtf8();
                int ret= QMessageBox::question(this,"共享文件",strNote.toUtf8());
                qDebug()<<"you choose yes or no"<<ret;
                if(QMessageBox::Yes==ret){//接收
                    PDU *respdu=mkPDU(pdu->uiMsgLen);
                    respdu->uiMsgType=ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND;
                    memcpy(respdu->caMsg,pdu->caMsg,pdu->uiMsgLen);

                    QString strName=TcpClient::getInstance().loginName();//接收者名字
                    qDebug()<<"strName recv:"<<strName;
                    strcpy(respdu->caData,strName.toStdString().c_str());
                    //发送
                    m_tcpSocket.write((char *)respdu,respdu->uiPDULen);
                    free(respdu);
                    respdu=NULL;
                }else {
                    QMessageBox::information(this,"共享文件夹","分享出错");
                }
            }

            break;
        }
        case ENUM_MSG_TYPE_MOVE_FILE_RESPOND://移动文件
        {
            QMessageBox::information(this,"移动文件",pdu->caData);
            break;
        }
        default:
            break;
        }
        //释放操作
        free(pdu);
        pdu=NULL;
    }else {
        //接收数据
        QByteArray buff=m_tcpSocket.readAll();
        m_file.write(buff);//把接收到的数据写入文件中
        qDebug()<<"保存文件中。。。";
        Book *pBook=OpeWidget::getInstance().getBook();
        pBook->m_iRecved+=buff.size();
        if(pBook->m_iTotal==pBook->m_iRecved){//下载完毕
            m_file.close();
            pBook->m_iTotal=0;
            pBook->m_iRecved=0;
            pBook->setDownLoadStatus(false);
        }else if(pBook->m_iTotal<pBook->m_iRecved){
            m_file.close();
            pBook->m_iTotal=0;
            pBook->m_iRecved=0;
            pBook->setDownLoadStatus(false);
            QMessageBox::critical(this,"下载文件","保存文件失败");
        }

    }


}
#if 0
//点击发送按钮
void TcpClient::on_pushButton_clicked()
{
    QString strMsg=ui->lineEdit->text();//获取输入框内容
    if(!strMsg.isEmpty()){
        PDU *pdu=mkPDU(strMsg.size()+1);//数据的实际长度
        pdu->uiMsgType=8888;
        memcpy(pdu->caMsg,strMsg.toStdString().c_str(),strMsg.size());//拷贝数据到结构体中
        //test
        qDebug()<<(char *)(pdu->caMsg);
        m_tcpSocket.write((char *)pdu,pdu->uiPDULen);//tcpsocket进行发送消息
        free(pdu);
        pdu=NULL;
    }else{
        QMessageBox::warning(this,"send message","message is null！！！");
    }
}
#endif
//登录
void TcpClient::on_login_pb_clicked()
{
    //首先获取用户名与密码
    QString strName=ui->name_lin->text();
    QString strPwd=ui->pwd_lin->text();
    qDebug()<<strName<<strPwd;//test
    if(!strName.isEmpty() && !strPwd.isEmpty()){
        //临时保存name，
        m_strLoginName=strName;
        //添加用户名与密码填到PDU里面
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_REQUEST;//注册类型请求
        strncpy(pdu->caData,strName.toStdString().c_str(),32);//前面32位放用户名
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);//后面32位放密码
        qDebug()<<"name:"<<pdu->caData<<"pwd:"<<pdu->caData+32;//test
        //使用socket发送

        m_tcpSocket.write((char *)pdu,pdu->uiPDULen);//tcpsocket进行发送消息
        free(pdu);
        pdu=NULL;
    }else {
        QMessageBox::critical(this,"登录","登录操作有误，请重新输入！");
    }
}
//注册
void TcpClient::on_regist_pb_clicked()
{
    //首先获取用户名与密码
    QString strName=ui->name_lin->text();
    QString strPwd=ui->pwd_lin->text();
    qDebug()<<strName<<strPwd;//test
    if(!strName.isEmpty() && !strPwd.isEmpty()){
        //添加用户名与密码填到PDU里面
        PDU *pdu=mkPDU(0);
        pdu->uiMsgType=ENUM_MSG_TYPE_REGIST_REQUEST;//注册类型请求
        strncpy(pdu->caData,strName.toStdString().c_str(),32);//前面32位放用户名
        strncpy(pdu->caData+32,strPwd.toStdString().c_str(),32);//后面32位放密码
        qDebug()<<"name:"<<pdu->caData<<"pwd:"<<pdu->caData+32;//test
        //使用socket发送
        m_tcpSocket.write((char *)pdu,pdu->uiPDULen);//tcpsocket进行发送消息
        free(pdu);
        pdu=NULL;
    }else {
        QMessageBox::critical(this,"注册","注册信息有误，请重新输入！");
    }
}
//注销
void TcpClient::on_cancel_pb_clicked()
{
    //把两个输入框进行清空
    ui->name_lin->clear();
    ui->pwd_lin->clear();
}
