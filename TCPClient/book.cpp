#include "book.h"
#include <QInputDialog>//输入框
#include <QMessageBox>
#include "protocol.h"
#include "tcpclient.h"
#include  <QFileDialog>
#include "opewidget.h"
#include "sharefile.h"
Book::Book(QWidget *parent) : QWidget(parent)
{

    m_bDownload=false;//默认为不下载
    m_pTimer=new QTimer;//定时器
    m_strEnterDir.clear();//清空当前进入文件夹信息
    m_pBoookListW=new QListWidget ;//显示文件列表

    m_pReturnPB=new QPushButton("返回");//返回
    m_pCreateDirPB=new QPushButton("创建文件夹");//创建文件夹
    m_pDelDriPB=new QPushButton("删除文件夹");//删除文件夹
    m_pRnamePB=new QPushButton("重命名文件夹");//重命名文件夹
    m_pFlushFilePB=new QPushButton("刷新文件夹");//刷新文件夹


    m_pUploadPB=new QPushButton("上传文件");//上传文件
    m_pDelFilePB=new QPushButton("删除文件");//删除文件
    m_pDownloadPB=new QPushButton("下载文件");//下载文件
    m_pShareFilePB=new QPushButton("分享文件");//分享文件
    m_pMoveFilePB=new QPushButton("移动文件");//移动文件
    m_pSelectDirPB=new QPushButton("目标目录");//移动目标目录
    m_pSelectDirPB->setEnabled(false);//默认不选

    //中间文件夹操作部分
    QVBoxLayout *pDriVBL=new QVBoxLayout;
    pDriVBL->addWidget(m_pReturnPB);
    pDriVBL->addWidget(m_pCreateDirPB);
    pDriVBL->addWidget(m_pDelDriPB);
    pDriVBL->addWidget(m_pRnamePB);
    pDriVBL->addWidget(m_pFlushFilePB);

    //靠右文件操作部分
    QVBoxLayout *pFileVBL=new QVBoxLayout;
    pFileVBL->addWidget(m_pUploadPB);
    pFileVBL->addWidget(m_pDelFilePB);
    pFileVBL->addWidget(m_pDownloadPB);
    pFileVBL->addWidget(m_pShareFilePB);
    pFileVBL->addWidget(m_pMoveFilePB);
    pFileVBL->addWidget(m_pSelectDirPB);

    //三个部分合成一个水平布局
    QHBoxLayout * pMain=new QHBoxLayout;
    pMain->addWidget(m_pBoookListW);
    pMain->addLayout(pDriVBL);
    pMain->addLayout(pFileVBL);

    //设置总体布局
    setLayout(pMain);

    /*----------------------关联信号槽------------------------*/
    connect(m_pCreateDirPB,SIGNAL(clicked(bool)),this,SLOT(createDir()));

    connect(m_pFlushFilePB,SIGNAL(clicked(bool)),this,SLOT(flushFile()));

    connect(m_pDelDriPB,SIGNAL(clicked(bool)),this,SLOT(delDir()));

    connect(m_pRnamePB,SIGNAL(clicked(bool)),this,SLOT(renameFile()));

    //双击进入文件列表
    connect(m_pBoookListW,SIGNAL(doubleClicked(QModelIndex)),this,SLOT(enterDir(QModelIndex)));

    connect(m_pReturnPB,SIGNAL(clicked(bool)),this,SLOT(returnPre()));

    connect(m_pUploadPB,SIGNAL(clicked(bool)),this,SLOT(uploadFile()));

    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(uploadFileDate()));

    connect(m_pDelFilePB,SIGNAL(clicked(bool)),this,SLOT(delRegFile()));

    connect(m_pDownloadPB,SIGNAL(clicked(bool)),this,SLOT(downloadFile()));

    connect(m_pShareFilePB,SIGNAL(clicked(bool)),this,SLOT(shareFile()));

    connect(m_pMoveFilePB,SIGNAL(clicked(bool)),this,SLOT(moveFile()));

    connect(m_pSelectDirPB,SIGNAL(clicked(bool)),this,SLOT(selectDestDir()));
}
//刷新文件列表
void Book::updateFileList(const PDU *pdu)
{
    if(NULL==pdu){
        return;
    }
    //把原来的内容清除
    QListWidgetItem * pItemTmp=NULL;
    int row=m_pBoookListW->count();
    while(m_pBoookListW->count()>0){
        pItemTmp= m_pBoookListW->item(row-1);
        m_pBoookListW->removeItemWidget(pItemTmp);//移除
        delete pItemTmp;
        row=row-1;
    }

    FileInfo * pFileInfo=NULL;
    //判断有多少个文件
    int iCount=pdu->uiMsgLen/sizeof(FileInfo);
    for(int i=0;i<iCount;i++){
        pFileInfo=(FileInfo *)(pdu->caMsg)+i;
        qDebug()<<"file name:"<<pFileInfo->caFileName<<"file type:"<<pFileInfo->iFileType;
        //添加图标，描述
        QListWidgetItem * pItem=new QListWidgetItem;
        if(pFileInfo->iFileType==0){//文件夹类型
            pItem->setIcon(QIcon(QPixmap(":/img/file.png")));
        }else if(pFileInfo->iFileType==1){//常规文件
            pItem->setIcon(QIcon(QPixmap(":/img/text.png")));
        }
        pItem->setText(pFileInfo->caFileName);//设置名字
        m_pBoookListW->addItem(pItem);
    }

}
//清空进入文件夹
void Book::clearEnterDir()
{
    m_strEnterDir.clear();
}
//获取当前文件
QString Book::enterDir()
{
    return m_strEnterDir;
}
//设置下载状态
void Book::setDownLoadStatus(bool status)
{
    m_bDownload=status;
}

//获取当前私有下载状态
bool Book::getDownloadStatus()
{
    return m_bDownload;
}
//获取当前私有文件路径
QString Book::getSaveFilePath()
{
    return m_strSaveFilePath;
}

//获得分享私有文件
QString Book::getShareFileName()
{
    return m_strShareFileName;
}
//创建文件夹操作
void Book::createDir()
{
    //创建文件名字
    QString strNewDir=QInputDialog::getText(this,"新文件夹","请输入文件夹名");
    if(!strNewDir.isEmpty()){
        if(strNewDir.size()>32){
            QMessageBox::warning(this,"新文件夹","文件夹名长度不符合");
        }else {
            //获取loginname和路径
            QString strName=TcpClient::getInstance().loginName();
            QString strCurPath=TcpClient::getInstance().curPath();
            //发送用户名、目录信息、新建文件夹名
            PDU *pdu=mkPDU(strCurPath.toUtf8().size()+1);
            pdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_REQUEST;//创建文件夹请求
            strncpy(pdu->caData,strName.toStdString().c_str(),strName.toUtf8().size());
            strncpy(pdu->caData+32,strNewDir.toStdString().c_str(),strNewDir.toUtf8().size());
            memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toUtf8().size());
            //发送
            TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
            free(pdu);
            pdu=NULL;
        }

    }else{
        QMessageBox::warning(this,"新建文件夹","文件夹有误，请重新输入！");
    }

}

//刷新文件夹
void Book::flushFile()
{
    //首先获取当前目录
    QString strCurPath=TcpClient::getInstance().curPath();
    qDebug()<<"strCurPath"<<strCurPath;
    PDU * pdu=mkPDU(strCurPath.toUtf8().size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_REQUEST;
   // strncpy((char *)(pdu->caMsg),strCurPath.toStdString().c_str(),strCurPath.toUtf8().size());
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toUtf8().size());
    //发送
    TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

//删除文件夹
void Book::delDir()
{
    //首先获取当前目录
    QString strCurPath=TcpClient::getInstance().curPath();
    //获得当前选项
    QListWidgetItem * pItem=m_pBoookListW->currentItem();
    if(NULL==pItem){
        QMessageBox::warning(this,"删除文件夹","删除文件夹为空");
    }else{
        QString strDelName=pItem->text();//获取名字
        PDU * pdu=mkPDU(strCurPath.toUtf8().size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_DEL_DIR_REQUEST;//删除路径请求
        strncpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.toUtf8().size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toUtf8().size());
        //发送
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }

}

//重命名
void Book::renameFile()
{
    //首先获取当前目录
    QString strCurPath=TcpClient::getInstance().curPath();
    //获取要修改的名字
    QListWidgetItem * pItem=m_pBoookListW->currentItem();
    if(NULL==pItem){
        QMessageBox::warning(this,"重命名","名字不能为空");
    }else{
        QString strOldName=pItem->text();//获取久的名字
        QString strNewName=QInputDialog::getText(this,"重命名","请输入新的名字");
        if(!strNewName.isEmpty()){
            PDU *pdu=mkPDU(strCurPath.toUtf8().size()+1);
            pdu->uiMsgType=ENUM_MSG_TYPE_RENAME_FILE_REQUEST;
            strncpy(pdu->caData,strOldName.toStdString().c_str(),strOldName.toUtf8().size());
            strncpy(pdu->caData+32,strNewName.toStdString().c_str(),strNewName.toUtf8().size());
            memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toUtf8().size());
            //发送
            TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
            free(pdu);
            pdu=NULL;
        }else{
            QMessageBox::warning(this,"重命名","名字不能为空");
        }
    }
}
//双击进入文件夹
void Book::enterDir(const QModelIndex &idnex)
{
    //拿到双击上面的文字
    QString strDirName=idnex.data().toString();
    qDebug()<<"strDirName"<<strDirName;
    m_strEnterDir=strDirName;
    QString strCurPath=TcpClient::getInstance().curPath();
    PDU *pdu=mkPDU(strCurPath.toUtf8().size()+1);
    pdu->uiMsgType=ENUM_MSG_TYPE_ENTER_DIR_REQUEST;//双击进入文件夹
    strncpy(pdu->caData,strDirName.toStdString().c_str(),strDirName.toUtf8().size());
    memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toUtf8().size());
    qDebug()<<"pdu->caMsg"<<pdu->caMsg;
    qDebug()<<"pdu->caData"<<pdu->caData;
    //发送
    TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
    free(pdu);
    pdu=NULL;
}

//返回上一级目录
void Book::returnPre()
{
    //首先获取当前目录
    QString strCurPath=TcpClient::getInstance().curPath();
    QString strRootPath="./"+TcpClient::getInstance().loginName();//根目录
    //如果两个路径相同，则不能够再返回
    if(strCurPath==strRootPath){
        QMessageBox::warning(this,"返回","根目录不能再返回");
    }else{
        //返回上一级
        int index=strCurPath.lastIndexOf('/');//最后一个斜杠的位置
        strCurPath.remove(index,strCurPath.size()-index);
        qDebug()<<"return--->"<<strCurPath;
        TcpClient::getInstance().setCurPath(strCurPath);
        //清空:清空子文件夹上次操作的文件夹
        clearEnterDir();

        //调用完成之后刷新
        flushFile();
    }


}
//上传文件
void Book::uploadFile()
{
    //选择上传的路径
    m_strUploadFilePath= QFileDialog::getOpenFileName();
    qDebug()<<"文件上传 strUploadFilePath"<<m_strUploadFilePath;
    //只提取文件名，前面的文件路径就不要了
    if(!m_strUploadFilePath.isEmpty()){
        int index=m_strUploadFilePath.lastIndexOf('/');//最后一个斜杠的位置
        QString strFileName=m_strUploadFilePath.right(m_strUploadFilePath.size()-index-1);//这部分的理解就是：/aa/cc/dd  长度-索引位置-1（'/'所在位置）
        qDebug()<<"strFileName"<<strFileName;
        QFile file(m_strUploadFilePath);
        qint64 fileSize=file.size();//获取文件大小
        qDebug()<<"文件大小 fileSize："<<fileSize;
        //首先获取当前目录
        QString strCurPath=TcpClient::getInstance().curPath();

        PDU *pdu=mkPDU(strCurPath.toUtf8().size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST;
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toUtf8().size());
        sprintf(pdu->caData,"%s %lld",strFileName.toStdString().c_str(),fileSize);//把文件名和文件大小都放在cadata中
        //发送
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;

        //启动定时器
        m_pTimer->start(1000);
    }else{
        QMessageBox::warning(this,"上传文件","文件名不能为空");
    }
}
//上传文件的定时器处理:这部分是为了防止在上传文件名和大小的时候，连着传输文件内容，防止两块合并，内容在定时1秒后进行发送
void Book::uploadFileDate()
{
    m_pTimer->stop();
    QFile file(m_strUploadFilePath);
    if(!file.open(QIODevice::ReadOnly)){
        QMessageBox::warning(this,"上传文件","打开文件失败");
        return;
    }
    char * pBuffer= new char[4096];//4096的速率效率比较高
    qint64 ret=0;
    //循环读取数据
    while(true){
        ret=file.read(pBuffer,4096);//读取数据到pBuffer中，ret才是真正读取到数据大小
        if(ret>0 && ret<=4096){
            //发送数据给服务器
            TcpClient::getInstance().getTcpSocket().write(pBuffer,ret);
        }else if(ret==0){//读到文件末尾了
            break;
        }else{
            QMessageBox::warning(this,"上传文件","上传文件失败:读取失败");
            break;
        }
    }
    file.close();
    delete []pBuffer;
    pBuffer=NULL;
}
//删除常规文件
void Book::delRegFile()
{
    //首先获取当前目录
    QString strCurPath=TcpClient::getInstance().curPath();
    //获得当前选项
    QListWidgetItem * pItem=m_pBoookListW->currentItem();
    if(NULL==pItem){
        QMessageBox::warning(this,"删除文件","删除文件为空");
    }else{
        QString strDelName=pItem->text();//获取名字
        PDU * pdu=mkPDU(strCurPath.toUtf8().size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_DEL_FILE_REQUEST;//删除路径请求
        strncpy(pdu->caData,strDelName.toStdString().c_str(),strDelName.toUtf8().size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toUtf8().size());
        //发送
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }

    //删除完后应该再次调用刷新文件
    flushFile();
}

//下载文件
void Book::downloadFile()
{
    //获取文件名字

    //获得当前选项
    QListWidgetItem * pItem=m_pBoookListW->currentItem();
    if(NULL==pItem){
        QMessageBox::warning(this,"下载文件","请选择要下载的文件名");
    }else{
        //服务器发送文件信息过来，客户端需要制定保存的位置
        QString strSaveFilePath=QFileDialog::getSaveFileName().toUtf8();//选择路径
        if(strSaveFilePath.isEmpty()){
            QMessageBox::warning(this,"下载文件","下载路径有问题，请重新指定");
            m_strSaveFilePath.clear();
        }else{
            m_strSaveFilePath=strSaveFilePath;

        }

        //首先获取当前目录
        QString strCurPath=TcpClient::getInstance().curPath();
        PDU *pdu=mkPDU(strCurPath.toUtf8().size()+1);
        pdu->uiMsgType=ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST;
        QString strFileName=pItem->text().toUtf8();//获取列表上面的名字
        strncpy(pdu->caData,strFileName.toStdString().c_str(),strFileName.toUtf8().size());
        memcpy(pdu->caMsg,strCurPath.toStdString().c_str(),strCurPath.toUtf8().size());
        //发送
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;

    }
}
//共享文件
void Book::shareFile()
{
    //获得当前选项
    QListWidgetItem * pItem=m_pBoookListW->currentItem();
    if(NULL==pItem){
        QMessageBox::warning(this,"分享文件","请选择要分享的文件");
        return;
    }else{
        //获得分享的文件名
        m_strShareFileName=pItem->text().toUtf8();
    }
        //获得好友列表
        Friend * pFriend=OpeWidget::getInstance().getFriend();
        QListWidget * pFriendList=pFriend->getFriendLsit();//获取好友列表
        for(int i=0;i<pFriendList->count();i++){
            qDebug()<<pFriendList->item(i)->text();
        }
        //更新到好友列表上面
        ShareFile::getInstance().updateFriend(pFriendList);
        if(ShareFile::getInstance().isHidden()){
            ShareFile::getInstance().show();//显示
        }

}
//移动文件
void Book::moveFile()
{
    //获得要移动的文件
    QListWidgetItem *pCuritem =m_pBoookListW->currentItem();
    if(pCuritem!=NULL){
        m_strMoveFileName=pCuritem->text().toUtf8();
        //首先获取当前目录
        QString strCurPath=TcpClient::getInstance().curPath();
        //拼接
        m_strMoveFilePath=strCurPath.toUtf8()+'/'+m_strMoveFileName;
        m_pSelectDirPB->setEnabled(true);
    }else{
        QMessageBox::warning(this,"移动文件","请选择要移动的文件");
    }
}
//选择目的目录
void Book::selectDestDir()
{
    //获得要移动的文件
    QListWidgetItem *pCuritem =m_pBoookListW->currentItem();
    if(pCuritem!=NULL){
        //目的路径
        QString strDestDir=pCuritem->text();
        //首先获取当前目录
        QString strCurPath=TcpClient::getInstance().curPath();
        //拼接
        m_strDestDir=strCurPath+'/'+strDestDir;

        int srcLen=m_strMoveFilePath.toUtf8().size();
        int destLen=m_strDestDir.toUtf8().size();
        PDU *pdu=mkPDU(srcLen+destLen+2);
        pdu->uiMsgType=ENUM_MSG_TYPE_MOVE_FILE_REQUEST;
        sprintf(pdu->caData,"%d %d %s",srcLen,destLen,m_strMoveFileName.toStdString().c_str());
        memcpy(pdu->caMsg,m_strMoveFilePath.toStdString().c_str(),m_strMoveFilePath.toUtf8().size());
        memcpy((char *)(pdu->caMsg)+srcLen+1,m_strDestDir.toStdString().c_str(),m_strDestDir.toUtf8().size());
        //发送
        TcpClient::getInstance().getTcpSocket().write((char *)pdu,pdu->uiPDULen);
        free(pdu);
        pdu=NULL;
    }else{
        QMessageBox::warning(this,"移动文件","请选择要移动的文件");
    }
     m_pSelectDirPB->setEnabled(false);

}
