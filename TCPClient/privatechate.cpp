#include "privatechate.h"
#include "ui_privatechate.h"

#include "protocol.h"
#include "tcpclient.h"

#include <QMessageBox>
#include <string.h>
#include <wchar.h>
PrivateChate::PrivateChate(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PrivateChate)
{
    ui->setupUi(this);
}

PrivateChate::~PrivateChate()
{
    delete ui;
}
//单例模式：返回静态引用
PrivateChate &PrivateChate::getInstance()
{
    static PrivateChate instance;
    return instance;
}
//保存私有属性
void PrivateChate::setChatName(QString strName)
{
    m_strChatName=strName;//聊天对方的名字
    m_strLoginName=TcpClient::getInstance().loginName();//获取发送方名字
}
//更新显示聊天信息
void PrivateChate::updateMsg(const PDU *pdu)
{
    if(pdu==NULL){
        return ;
    }
    //拿到发送和接收方名字
    char caSendName[32]={'\0'};
    memcpy(caSendName,pdu->caData,32);

    //拼接消息
    //QString strMsg=QString("%1 say message:%2").arg(caSendName).arg((char *)(pdu->caMsg));
    QString strMsg = QString::fromUtf8("%1 say message:%2").arg(caSendName).arg((char *)(pdu->caMsg));

    //消息显示
    ui->showMsg_te->append(strMsg);
}
//点击发送消息
void PrivateChate::on_sendMsg_pb_clicked()
{
   //获取数据框消息
    ui->inputMsg_le->setInputMethodHints(Qt::ImhPreferUppercase);//设置字符集
    QString strMsg=ui->inputMsg_le->text();


    qDebug()<<"发送的消息为："<<strMsg;
    //清空输入框
    ui->inputMsg_le->clear();
    if(!strMsg.isEmpty()){
      PDU *pdu = mkPDU(strMsg.toUtf8().size()+1);
      pdu->uiMsgType=ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST;//私聊请求
      //数据封装:名字以及发送的消息进行封装
      memcpy(pdu->caData,m_strLoginName.toStdString().c_str(),m_strLoginName.size());
      memcpy(pdu->caData+32,m_strChatName.toStdString().c_str(),m_strChatName.size());
      memcpy(pdu->caMsg, strMsg.toStdString().c_str(), strMsg.toUtf8().size());

      //发送
      TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
      free(pdu);
      pdu=NULL;



    }else{
        QMessageBox::information(this,"私聊","发送消息为空");
    }
}
