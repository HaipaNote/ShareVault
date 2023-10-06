#include "sharefile.h"

ShareFile::ShareFile(QWidget *parent) : QWidget(parent)
{


    m_pSelectAllPB=new QPushButton("全选");//全选按钮
    m_pCancelSelectPB=new QPushButton("取消全选");//取消全选按钮

    m_pOKPB=new QPushButton("确定");//确定
    m_pCancelPB=new QPushButton("取消");//取消按钮

    m_pSA=new QScrollArea;//展示区域
    m_pFriendW=new QWidget;//显示好友
    m_pButtonGroup=new QButtonGroup(m_pFriendW);//管理全部好友
    m_pFriendWVBL=new QVBoxLayout(m_pFriendW);//好友列表垂直布局

    m_pButtonGroup->setExclusive(false);//支持多选

    /*---------------------布局------------------------------*/
    QHBoxLayout * pTopHBL=new QHBoxLayout;
    pTopHBL->addWidget(m_pSelectAllPB);
    pTopHBL->addWidget(m_pCancelSelectPB);
    pTopHBL->addStretch();//弹簧

    QHBoxLayout * pDownHBL=new QHBoxLayout;
    pDownHBL->addWidget(m_pOKPB);
    pDownHBL->addWidget(m_pCancelPB);

    QVBoxLayout *pMainVBL=new QVBoxLayout;
    pMainVBL->addLayout(pTopHBL);
    pMainVBL->addWidget(m_pSA);
    pMainVBL->addLayout(pDownHBL);

    setLayout(pMainVBL);

    /*--------------------关联信号槽---------------------------*/
    connect(m_pCancelSelectPB,SIGNAL(clicked(bool)),this,SLOT(cancelSelect()));//取消全选

    connect(m_pSelectAllPB,SIGNAL(clicked(bool)),this,SLOT(selectAll()));//全选

    connect(m_pOKPB,SIGNAL(clicked(bool)),this,SLOT(okShare()));

    connect(m_pCancelPB,SIGNAL(clicked(bool)),this,SLOT(cancelShare()));

}
//设置为单例模式
ShareFile &ShareFile::getInstance()
{
    static ShareFile instance;
    return instance;

}
//更新好友列表
void ShareFile::updateFriend(QListWidget *pFriendList)
{

    if(pFriendList==NULL){
        return;
    }
    QList<QAbstractButton *> preFriendList=m_pButtonGroup->buttons();//获得所有按钮选项
    QAbstractButton *temp=NULL;
    for(int i=0;i<preFriendList.size();i++){
        temp=preFriendList[i];
        //移除原来的内容:移除原来的指针
        m_pFriendWVBL->removeWidget(temp);
        m_pButtonGroup->removeButton(temp);
        preFriendList.removeOne(temp);
        delete temp;
        temp=NULL;
    }
    //将新的内容跟新上去
    QCheckBox * pCB=NULL;
    for(int i=0;i<pFriendList->count();i++){
        pCB=new QCheckBox(pFriendList->item(i)->text());
        m_pFriendWVBL->addWidget(pCB);
        m_pButtonGroup->addButton(pCB);
    }
    m_pSA->setWidget(m_pFriendW);
}
//取消选择
void ShareFile::cancelSelect()
{
    //获得选择的checkbox
    QList<QAbstractButton *> cbList=m_pButtonGroup->buttons();
    for(int i=0;i<cbList.size();i++){
        if(cbList[i]->isChecked()){
            cbList[i]->setChecked(false);
        }
    }
}
//全选
void ShareFile::selectAll()
{
    //获得选择的checkbox
    QList<QAbstractButton *> cbList=m_pButtonGroup->buttons();
    for(int i=0;i<cbList.size();i++){
        if(!cbList[i]->isChecked()){
            cbList[i]->setChecked(true);
        }
    }
}
//确定
void ShareFile::okShare()
{
    //获得分享者
    QString strName=TcpClient::getInstance().loginName();
    //获得当前路径
    QString strCurPath=TcpClient::getInstance().curPath();
    //分享的文件名
    QString strShareFileName=OpeWidget::getInstance().getBook()->getShareFileName();

    //拼接路径和共享的文件
    QString strPath=strCurPath+"/"+strShareFileName;

    //接收者:计算有多少个人上了
    int num=0;//选中的人
    QList<QAbstractButton *> cbList=m_pButtonGroup->buttons();
    for(int i=0;i<cbList.size();i++){
        if(cbList[i]->isChecked()){

           num++;
        }
    }

    PDU *pdu=mkPDU(32*num+strPath.toUtf8().size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_SHARE_FILE_REQUEST;
    sprintf(pdu->caData,"%s %d",strName.toStdString().c_str(),num);
    int j=0;
    for (int  i=0;i<cbList.size();i++) {
        if(cbList[i]->isChecked()){
            memcpy((char *)(pdu->caMsg)+j*32,cbList[i]->text().toStdString().c_str(),cbList[i]->text().toUtf8().size());
            j++;
        }
    }
    memcpy((char *)(pdu->caMsg)+num*32,strPath.toStdString().c_str(),strPath.toUtf8().size());
    //发送
    TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}
//取消
void ShareFile::cancelShare()
{
    //隐藏窗口
    hide();
}
