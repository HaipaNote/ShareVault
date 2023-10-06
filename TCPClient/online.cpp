#include "online.h"
#include "ui_online.h"
#include "tcpclient.h"
#include <QDebug>
Online::Online(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Online)
{
    ui->setupUi(this);
}

Online::~Online()
{
    delete ui;
}
//显示在线人名
void Online::showUsr(PDU *pdu)
{
    if(pdu==NULL){
        return ;
    }
    //提取人名
    uint uiSize=pdu->uiMsgLen/32;
    char caTmp[32];//使用临时数组存放数据
    for (int i=0;i<uiSize;i++) {
        memcpy(caTmp,(char *)(pdu->caMsg)+i*32,32);
        //test
        qDebug()<<"caTmp:"<<caTmp;
        ui->online_lw->addItem(caTmp);//显示数据
    }
}
//添加好友实现
void Online::on_addFriend_pb_clicked()
{
   //获得左边lw中的好友信息
   QListWidgetItem * pitem=ui->online_lw->currentItem();
   //test
   qDebug()<<"tiem :"<<pitem->text();
   //拿到行列名
   QString strPerUsrName=pitem->text();
   //获得登录名
   QString strLoginName=TcpClient::getInstance().loginName();
   PDU * pdu=mkPDU(0);
   pdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_REQUEST;//添加好友类型
   //将数据放入pdu中
   memcpy(pdu->caData,strPerUsrName.toStdString().c_str(),strPerUsrName.size());
   memcpy(pdu->caData+32,strLoginName.toStdString().c_str(),strLoginName.size());
   //发送
   TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
   free(pdu);
   pdu=NULL;

}
