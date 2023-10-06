#include "mytcpsocket.h"
#include "opedb.h"
#include <QDebug>
#include "mytcpserver.h"
#include <QDir>
#include <QFileInfoList>
MyTcpSocket::MyTcpSocket()
{
    //上传文件状态默认为不上传
    m_bUpload=false;
    //初始化定时器
    m_pTimer=new QTimer;

    //当有数据来的时候，就会产生socket，而直接封装好socket就可以避免每次都新产生一个socket
    connect(this,SIGNAL(readyRead()),this,SLOT(recvMsg()));

    //下线关联
    connect(this,SIGNAL(disconnected()),this,SLOT(clientOfflinet()));

    connect(m_pTimer,SIGNAL(timeout()),this,SLOT(sendFileToClient()));


}
//获取私有变量m_strName
QString MyTcpSocket::getName()
{
    return m_strName;
}
//拷贝文件夹
void MyTcpSocket::copyDir(QString strSrcDir, QString strDestDir)
{
    //目标文件
    QDir dir;
    dir.mkdir(strDestDir);//创建目标文件

    //打开原目录，遍历拷贝
    dir.setPath(strSrcDir);
    QFileInfoList fileInfoList=dir.entryInfoList();
    //临时变量
    QString srcTmp;
    QString destTmp;
    for (int i=0;i<fileInfoList.size();i++) {
        qDebug()<<"file name:"<<fileInfoList[i].fileName();
        //进行判断，如果是常规文件，就直接拷贝，若是文件目录，则递归拷贝
        if(fileInfoList[i].isFile()){
            //普通文件就直接使用拷贝
            srcTmp=strSrcDir+'/'+ fileInfoList[i].fileName();
            destTmp=strDestDir+'/'+ fileInfoList[i].fileName();
            QFile::copy(srcTmp,destTmp);
        }else if (fileInfoList[i].isDir()) {
            //根目录就先判断
            if(QString(".")== fileInfoList[i].fileName() || QString("..")== fileInfoList[i].fileName()){
                continue;
            }
            srcTmp=strSrcDir+'/'+ fileInfoList[i].fileName();
            destTmp=strDestDir+'/'+ fileInfoList[i].fileName();
            copyDir(srcTmp,destTmp);
        }
    }

}

//接收消息
void MyTcpSocket::recvMsg()
{
    if(!m_bUpload){//如果不是上传文件的状态则，以PDU的类型来解析文件
    //test
    qDebug()<<this->bytesAvailable();//当前可读的数据大小
    uint uiPDULen=0;
    this->read((char *)&uiPDULen,sizeof(uint));//先收4字节的长度，这四个字节就包含了整个数据包大小
    uint uiMsgLen=uiPDULen-sizeof(PDU);//实际消息长度=总长（协议大小）-固定长度（就是除动态数组部分）
    PDU *pdu=mkPDU(uiMsgLen);//接受剩余数据长度
    this->read((char *)pdu+sizeof(uint),uiPDULen-sizeof(uint));//往后偏移
    qDebug()<<pdu->uiMsgType<<(char *)(pdu->caMsg);//test

    qDebug()<<"name:"<<pdu->caData<<"pwd:"<<pdu->caData+32<<"msgType:"<<pdu->uiMsgType;//test

    //首先判断一下消息类型
    switch(pdu->uiMsgType){
    //将数据添加到数据库
    case ENUM_MSG_TYPE_REGIST_REQUEST://注册类型的请求
    {
        //接收用户名与密码
        char caName[32]={'\0'};
        char caPwd[32]={'\0'};
        strncpy(caName,pdu->caData,32);
        strncpy(caPwd,pdu->caData+32,32);
        bool ret=OpeDB::getInstance().handleRegist(caName,caPwd);//处理请求
        //注册完成后回复消息
        PDU * respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_REGIST_RESPOND;
        if(ret){
            strcpy(respdu->caData,REGIST_OK);

            //创建文件夹
            QDir dir;
            qDebug()<<"make user dir:"<<dir.mkdir(QString("./%1").arg(caName));//以用户名为文件夹名字


        }else{
            strcpy(respdu->caData,REGIST_FAILED);
        }
        //本身就是socket，直接发送数据即可，该部分为注册的一个回复
        write((char *)respdu,respdu->uiPDULen);//tcpsocket进行发送消息
        free(respdu);
        respdu=NULL;

        qDebug()<<caName<<caPwd<<pdu->uiMsgType;//test

    }
    case ENUM_MSG_TYPE_LOGIN_REQUEST://登录类型的请求
    {
        //接收用户名与密码
        char caName[32]={'\0'};
        char caPwd[32]={'\0'};
        strncpy(caName,pdu->caData,32);
        strncpy(caPwd,pdu->caData+32,32);
        bool ret=OpeDB::getInstance().handleLogin(caName,caPwd);//处理请求

        //登录完成后回复消息
        PDU * respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_LOGIN_RESPOND;
        if(ret){
            strcpy(respdu->caData,LOGIN_OK);
            m_strName=caName;//记录登录的用户名
        }else{
            strcpy(respdu->caData,LOGIN_FAILED);
        }

        //本身就是socket，直接发送数据即可，该部分为登录的一个回复
        write((char *)respdu,respdu->uiPDULen);//tcpsocket进行发送消息
        free(respdu);
        respdu=NULL;
        break;
        qDebug()<<caName<<caPwd<<pdu->uiMsgType;//test
    }
    case ENUM_MSG_TYPE_ALL_ONLINE_REQUEST://所有在线人数请求
    {
        QStringList ret=OpeDB::getInstance().handleAllOnline();//获取所有在线人数
        //获取名字的总的需要占多少空间
        uint uiMsgLen=ret.size()*32;
        PDU * respdu=mkPDU(uiMsgLen);
        respdu->uiMsgType=ENUM_MSG_TYPE_ALL_ONLINE_RESPOND;
        for (int i=0;i<ret.size();i++) {
            //在respdu->caMsg中，数据来一个就往后存一个，不断迭代往后
            memcpy((char *)(respdu->caMsg)+i*32,ret.at(i).toStdString().c_str(),ret.at(i).size());
            //test
            qDebug()<<"caMsg:"<<respdu->caMsg;
        }
        //发送
        write((char *)respdu,respdu->uiPDULen);//tcpsocket进行发送消息
        free(respdu);
        respdu=NULL;

        break;
    }
    case ENUM_MSG_TYPE_SEARCH_USR_REQUEST://查找用户请求
    {
        int ret=OpeDB::getInstance().handleSearchUsr(pdu->caData);
        PDU * respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_SEARCH_USR_RESPOND;
        if(ret==-1){
            strcpy(respdu->caData,SEARCH_USR_NO);
        }else if(ret ==1){
            strcpy(respdu->caData,SEARCH_USR_ONLINE);
        }else if(ret ==0){
            strcpy(respdu->caData,SEARCH_USR_OFFLINE);
        }
        //发送
        write((char *)respdu,respdu->uiPDULen);//tcpsocket进行发送消息
        free(respdu);
        respdu=NULL;
        break;

    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REQUEST://添加好友请求
    {
        //1.判断这个人是不是好友
        //2.判断这个人在不在线
        //3.在线，之后进行处理

        //获取对方名字以及我的名字
        char caPreName[32]={'\0'};
        char caName[32]={'\0'};
        strncpy(caPreName,pdu->caData,32);
        strncpy(caName,pdu->caData+32,32);
        int ret=OpeDB::getInstance().handleAddFriend(caPreName,caName);
        PDU *respdu=NULL;
        if(ret==-1){//添加好友不存在
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;//添加好友信号
            strcpy(respdu->caData,UNKNOW_ERROR);

            //发送
            write((char *)respdu,respdu->uiPDULen);//tcpsocket进行发送消息
            free(respdu);
            respdu=NULL;
        }else if(ret==0){
            //双方已经是好友
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,EXISTED_FRIEND);

            //发送
            write((char *)respdu,respdu->uiPDULen);//tcpsocket进行发送消息
            free(respdu);
            respdu=NULL;

        }else if(ret==1){
            //双方已经是好友，需要进行转发
            MyTcpServer::getInstance().resend(caPreName,pdu);//转发


        }else if(ret==2){
            //对方不在线
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,ADD_FRIEND_OFFLINE);
            //发送
            write((char *)respdu,respdu->uiPDULen);//tcpsocket进行发送消息
            free(respdu);
            respdu=NULL;
        }else if(ret==3){
            //对方不存在
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ADD_FRIEND_RESPOND;
            strcpy(respdu->caData,ADD_FRIEND_NO_EXIST);
            //发送
            write((char *)respdu,respdu->uiPDULen);//tcpsocket进行发送消息
            free(respdu);
            respdu=NULL;
        }

        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_AGREE:
    {
        char caPerName[32] = {'\0'};
        char caName[32] = {'\0'};
        strncpy(caPerName, pdu->caData, 32);
        strncpy(caName, pdu->caData+32, 32);
        OpeDB::getInstance().handleAgreeAddFriend(caPerName, caName);

        MyTcpServer::getInstance().resend(caName, pdu);
        break;
    }
    case ENUM_MSG_TYPE_ADD_FRIEND_REFUSE:
    {
        char caName[32] = {'\0'};
        strncpy(caName, pdu->caData+32, 32);
        MyTcpServer::getInstance().resend(caName, pdu);
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST://刷新好友请求
    {
        char caName[32]={'\0'};
        strncpy(caName,pdu->caData,32);
        QStringList  ret=OpeDB::getInstance().handleFlushFriend(caName);
        //将产生的数据返回给客户端
        uint uiMsgLen=ret.size()*32;//一个人名32字节
        PDU *respdu=mkPDU(uiMsgLen);
        respdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND;//刷新好友回复
        for (int i=0;i<ret.size();i++) {
            memcpy((char *)(respdu->caMsg)+i*32,ret.at(i).toStdString().c_str(),ret.at(i).size());

        }
        //发送
        write((char *)respdu,respdu->uiPDULen);//tcpsocket进行发送消息
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST://删除好友请求
    {
        //先获取我的以及朋友的名字
        //获取对方名字以及我的名字
        char caSelfName[32]={'\0'};
        char caFriendName[32]={'\0'};
        strncpy(caSelfName,pdu->caData,32);
        strncpy(caFriendName,pdu->caData+32,32);

        OpeDB::getInstance().handleDelFriend(caSelfName,caFriendName);


        //给双方消息提示:回复给删除方
        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND;//删除好友回复
        strcpy(respdu->caData,DEL_FRIEND_OK);
        //发送
        write((char *)respdu,respdu->uiPDULen);//tcpsocket进行发送消息
        free(respdu);
        respdu=NULL;

        //给删除方的一个提示
        MyTcpServer::getInstance().resend(caFriendName,pdu);//转发
        break;


    }
    case ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST://私聊请求
    {
        //首先找到对方的socket，然后进行消息转发
        char caPerName[32]={'\0'};
        memcpy(caPerName,pdu->caData+32,32);//先获取对象名
        //test
        char caName[32] = {'\0'};
        memcpy(caName, pdu->caData, 32);
        qDebug() << caName << "-->" << caPerName << (char*)(pdu->caMsg);
        MyTcpServer::getInstance().resend(caPerName,pdu);//转发
        break;
    }
    case ENUM_MSG_TYPE_GROUP_CHAT_REQUEST://群聊请求
    {
        //先知道那些好友在线，再进行转发
        //获得发送方名，获得好友名
        char caName[32]={'\0'};
        strncpy(caName,pdu->caData,32);
        QStringList  onlineFriend=OpeDB::getInstance().handleFlushFriend(caName);
        //转发
        QString tmp;//临时变量存储转发好友
        for(int i=0;i<onlineFriend.size();i++){
            tmp=onlineFriend.at(i);
            MyTcpServer::getInstance().resend(tmp.toStdString().c_str(),pdu);//转发
        }
        break;
    }
    case ENUM_MSG_TYPE_CREATE_DIR_REQUEST://创建文件夹请求
    {
        //判断当前目录是否存在
        QDir dir;
        //当前目录
        QString strCurPath=QString("%1").arg((char *)(pdu->caMsg));
        qDebug()<<"strCurPath"<<strCurPath;
        bool ret=dir.exists(strCurPath);
        //回复客户端
        PDU * respdu=NULL;

        if(ret){//当前目录存在
            //把新的文件夹在当前文件夹下进行查找，查看文件名是否同名，同名不能够进行创建
            char caNewDir[32]={'\0'};
            memcpy(caNewDir,pdu->caData+32,32);
            //拼接当前路径
            QString strNewPath=strCurPath+"/"+caNewDir;
            qDebug()<<strNewPath;
            ret=dir.exists(strNewPath);
            qDebug()<<"----->"<<ret;

            if(ret){//如果文件名存在，则说明存在该文件名，不能创建
                respdu=mkPDU(0);
                respdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                strcpy(respdu->caData,FILE_NAME_EXIST);
            }else {//不存在，可以进行创建
                //创建
                dir.mkdir(strNewPath);
                respdu=mkPDU(0);
                respdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
                strcpy(respdu->caData,CREATE_DIR_OK);
            }

        }else{//当前目录不存在
            qDebug()<<"当前目录不存在";
            //回复
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_CREATE_DIR_RESPOND;
            strcpy(respdu->caData,DIR_NO_EXIST);

        }
        //发送消息
        write((char *)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_FLUSH_FILE_REQUEST://刷新文件请求
    {
        //拿到路径
        char * pCurPath=new char[pdu->uiMsgLen];
        memcpy(pCurPath,pdu->caMsg,pdu->uiMsgLen);
        qDebug()<<"pCurPath"<<pCurPath;
        //遍历目录信息
        QDir dir(pCurPath);
        QFileInfoList fileInfoList=dir.entryInfoList();
        //先知道文件个数
        int iFileCount=fileInfoList.size();
        //大小计算：文件个数*结构体大小
        PDU * respdu=mkPDU((iFileCount)*sizeof (FileInfo));
        respdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
        //使用临时指针，让其不断指向下一个内容
        FileInfo * pFileInfo=NULL;
        QString strFileName;//保存临时文件名

        //遍历
        for (int i=0;i<iFileCount;i++) {
//            //把.和..的情况进行处理，隐藏这两个部分
//            if(QString(".")==fileInfoList[i].fileName()||QString("..")==fileInfoList[i].fileName()){
//                continue;
//            }
            pFileInfo=(FileInfo *)(respdu->caMsg)+i;//关键点：把这个消息内容当做一个结构体来使用
            strFileName=fileInfoList[i].fileName().toUtf8();//提取名字
            //memcpy(pFileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.size());
            memcpy(pFileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.toUtf8().size());//更改部分
            qDebug()<<"caFile name:"<<pFileInfo->caFileName;
            //判断类型
            if(fileInfoList[i].isDir()){
                pFileInfo->iFileType=0;//0表示一个文件夹
            }else{
                 pFileInfo->iFileType=1;//1表示一个常规文件
            }

        }
        //发送消息
        write((char *)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_DEL_DIR_REQUEST://删除文件夹
    {

        //拼接路径
        char caName[32]={'\0'};
        strcpy(caName,pdu->caData);//把文件名提出来
        char * pPath=new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);//把文件路径提出来
        //拼接
        QString strPath=QString("%1/%2").arg(pPath).arg(caName);
        qDebug()<<"strPath"<<strPath;
        //判断删除的是路径还是常规文件
        QFileInfo fileInfo(strPath);
        bool ret=false;
        if(fileInfo.isDir()){//文件夹
            QDir dir;
            dir.setPath(strPath);
            ret=dir.removeRecursively();
          }else if(fileInfo.isFile()){//常规文件
            ret=false;
        }
        PDU * respdu=NULL;
        if(ret){

            respdu=mkPDU(strlen(DEL_DIR_OK)+1);
            respdu->uiMsgType=ENUM_MSG_TYPE_DEL_DIR_RESPOND;
            memcpy(respdu->caData,DEL_DIR_OK,strlen(DEL_DIR_OK));

        }else {
            respdu=mkPDU(strlen(DEL_DIR_FAILURED)+1);
            respdu->uiMsgType=ENUM_MSG_TYPE_DEL_DIR_RESPOND;
            memcpy(respdu->caData,DEL_DIR_FAILURED,strlen(DEL_DIR_FAILURED));
        }

        //发送消息
        write((char *)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;

        break;
    }
    case ENUM_MSG_TYPE_RENAME_FILE_REQUEST://重命名
    {
        //提取新旧名字以及路径
        char caOldName[32]={'\0'};
        char caNewName[32]={'\0'};
        strncpy(caOldName,pdu->caData,32);
        strncpy(caNewName,pdu->caData+32,32);

        char * pPath=new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        //拼接
        QString strOldPath=QString("%1/%2").arg(pPath).arg(caOldName);
        QString strNewPath=QString("%1/%2").arg(pPath).arg(caNewName);

        //重命名操作
        QDir dir;
        bool ret=dir.rename(strOldPath,strNewPath);
        PDU * respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_RENAME_FILE_RESPOND;
        if(ret){
            strcpy(respdu->caData,RENAME_FILE_OK);
        }else{
            strcpy(respdu->caData,RENAME_FILE_FAILURED);
        }
        //发送消息
        write((char *)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;
        break;
    }
    case ENUM_MSG_TYPE_ENTER_DIR_REQUEST://进入文件夹
    {
        //提取新旧名字以及路径
        char caEnterName[32]={'\0'};
        strncpy(caEnterName,pdu->caData,32);

        char * pPath=new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        QString strPath=QString("%1/%2").arg(pPath).arg(caEnterName);

        QFileInfo fileInfo(strPath);
        PDU *respdu=NULL;
        if(fileInfo.isDir()){
            //获取当前目录的全部信息
            //遍历目录信息
            QDir dir(strPath);
            QFileInfoList fileInfoList=dir.entryInfoList();
            //先知道文件个数
            int iFileCount=fileInfoList.size();
            //大小计算：文件个数*结构体大小
            respdu=mkPDU((iFileCount)*sizeof (FileInfo));
            respdu->uiMsgType=ENUM_MSG_TYPE_FLUSH_FILE_RESPOND;
            //使用临时指针，让其不断指向下一个内容
            FileInfo * pFileInfo=NULL;
            QString strFileName;//保存临时文件名

            //遍历
            for (int i=0;i<iFileCount;i++) {
    //            //把.和..的情况进行处理，隐藏这两个部分
    //            if(QString(".")==fileInfoList[i].fileName()||QString("..")==fileInfoList[i].fileName()){
    //                continue;
    //            }
                pFileInfo=(FileInfo *)(respdu->caMsg)+i;//关键点：把这个消息内容当做一个结构体来使用
                strFileName=fileInfoList[i].fileName().toUtf8();//提取名字
                memcpy(pFileInfo->caFileName,strFileName.toStdString().c_str(),strFileName.toUtf8().size());
                qDebug()<<"input file name:"<<pFileInfo->caFileName;
                //判断类型
                if(fileInfoList[i].isDir()){
                    pFileInfo->iFileType=0;//0表示一个文件夹
                }else{
                     pFileInfo->iFileType=1;//1表示一个常规文件
                }

            }
            //发送消息
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;

        }else if(fileInfo.isFile()){
            respdu=mkPDU(0);
            respdu->uiMsgType=ENUM_MSG_TYPE_ENTER_DIR_RESPOND;
            strncpy(respdu->caData,ENTER_DIR_FAILURED,strlen(ENTER_DIR_FAILURED));
            //发送消息
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }
        break;

    }
    case ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST://上传文件
    {
        //提取新旧名字以及路径
        char caFileName[128]={'\0'};
        qint64 fileSize=0;
        sscanf(pdu->caData,"%s %lld",caFileName,&fileSize);//提取文件名和大小
        char * pPath=new char[pdu->uiMsgLen];//提取文件路径
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);
        QString strPath=QString("%1/%2").arg(pPath).arg(caFileName);//再次拼接
        qDebug()<<"上传路径：strPath"<<strPath;
        delete []pPath;
        pPath=NULL;

        m_file.setFileName(strPath);
        if(m_file.open(QIODevice::WriteOnly)){//以写的方式打开文件，若文件不存在，则自动创建文件
          m_bUpload=true;//打开上传
          m_iTotal=fileSize;//总的大小
          m_iRecved=0;//上传文件数为0
        }else {

        }

        break;
    }
    case ENUM_MSG_TYPE_DEL_FILE_REQUEST://删除文件
    {
        //拼接路径
        char caName[32]={'\0'};
        strcpy(caName,pdu->caData);//把文件名提出来
        char * pPath=new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);//把文件路径提出来
        //拼接
        QString strPath=QString("%1/%2").arg(pPath).arg(caName);
        qDebug()<<"strPath"<<strPath;
        //判断删除的是路径还是常规文件
        QFileInfo fileInfo(strPath);
        bool ret=false;
        if(fileInfo.isDir()){//文件
             ret=false;
          }else if(fileInfo.isFile()){//常规文件

            QDir dir;
            ret=dir.remove(strPath);
        }
        PDU * respdu=NULL;
        if(ret){

            respdu=mkPDU(strlen(DEL_FILE_OK)+1);
            respdu->uiMsgType=ENUM_MSG_TYPE_DEL_FILE_RESPOND;
            memcpy(respdu->caData,DEL_FILE_OK,strlen(DEL_FILE_OK));

        }else {
            respdu=mkPDU(strlen(DEL_FILE_FAILURED)+1);
            respdu->uiMsgType=ENUM_MSG_TYPE_DEL_FILE_RESPOND;
            memcpy(respdu->caData,DEL_FILE_FAILURED,strlen(DEL_FILE_FAILURED));
        }

        //发送消息
        write((char *)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;

        break;
    }
    case ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST://下载文件
    {
        //拼接路径
        char caFileName[32]={'\0'};
        strcpy(caFileName,pdu->caData);//把文件名提出来
        char * pPath=new char[pdu->uiMsgLen];
        memcpy(pPath,pdu->caMsg,pdu->uiMsgLen);//把文件路径提出来
        //拼接
        QString strPath=QString("%1/%2").arg(pPath).arg(caFileName);
        qDebug()<<"strPath"<<strPath;
        delete []pPath;
        pPath=NULL;

        QFileInfo fileInfo(strPath.toUtf8());
        qint64 fileSize=fileInfo.size();//获取文件大小
        PDU * respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND;
        sprintf(respdu->caData,"%s %lld",caFileName,fileSize);

        //发送消息
        write((char *)respdu,respdu->uiPDULen);
        free(respdu);
        respdu=NULL;

        //打开文件
        m_file.setFileName(strPath);
        m_file.open(QIODevice::ReadOnly);
        m_pTimer->start(1000);//延迟1s
        break;
    }
    case ENUM_MSG_TYPE_SHARE_FILE_REQUEST://共享文件夹
    {
        //分别提取出接收者名字，分别给他们发送。接收者有两种选择：
        //1.选择接受：共享文件就需要拷贝一份到接收者的目录下
        //2.不接收，不做处理

        char caSendName[32] = {'\0'};
        int num = 0;
        sscanf(pdu->caData, "%s%d", caSendName, &num);
        int size = num*32;
        PDU *respdu = mkPDU(pdu->uiMsgLen-size);
        respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_NOTE;
        strcpy(respdu->caData, caSendName);
        memcpy(respdu->caMsg, (char*)(pdu->caMsg)+size, pdu->uiMsgLen-size);

        char caRecvName[32] = {'\0'};
        for (int i=0; i<num; i++)
        {
            memcpy(caRecvName, (char*)(pdu->caMsg)+i*32, 32);
            MyTcpServer::getInstance().resend(caRecvName, respdu);
        }
        free(respdu);
        respdu = NULL;

        respdu = mkPDU(0);
        respdu->uiMsgType = ENUM_MSG_TYPE_SHARE_FILE_RESPOND;
        strcpy(respdu->caData, "share file ok");
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;

        break;


    }
    case ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND://通知回复
    {
        //拷贝到接收者名字目录下
        QString strRecvPath=QString("./%1").arg(pdu->caData);
        QString strShareFilePath=QString("%1").arg((char *)(pdu->caMsg));
        //提取文件名字
        int  index=strShareFilePath.lastIndexOf('/');
        QString strFileName=strShareFilePath.right(strShareFilePath.size()-index-1);
        //拼接
        strRecvPath=strRecvPath+'/'+strFileName;
        qDebug()<<"strFileName:"<<strFileName;
        qDebug()<<"note strRecvPath:"<<strRecvPath;
        qDebug()<<"note strShareFilePath:"<<strShareFilePath;
        //文件拷贝
        QFileInfo fileInfo(strShareFilePath);
        if(fileInfo.isFile()){
            QFile::copy(strShareFilePath.toUtf8(),strRecvPath.toUtf8());
            qDebug()<<"copy file successs!!!!";
        }else if (fileInfo.isDir()) {

            copyDir(strShareFilePath.toUtf8(),strRecvPath.toUtf8());
             qDebug()<<"copy dir successs!!!!";
        }

        break;
    }
      case  ENUM_MSG_TYPE_MOVE_FILE_REQUEST://移动文件
    {
        //获得两个路径大小和文件大小
        char caFileName[32]={'\0'};
        int srcLen=0;
        int destLen=0;
        sscanf(pdu->caData,"%d%d%s",&srcLen,&destLen,caFileName);

        //产生两个空间吧路径拷贝出来
        char *pSrcPath=new char[srcLen+1];
        char *pDestPath=new char[destLen+1+32];
        memset(pSrcPath,'\0',srcLen+1);
        memset(pDestPath,'\0',destLen+1+32);

        memcpy(pSrcPath,pdu->caMsg,srcLen);
        memcpy(pDestPath,(char *)(pdu->caMsg)+srcLen+1,destLen);

        QFileInfo fileInfo(pDestPath);

        PDU *respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_MOVE_FILE_RESPOND;

        if(fileInfo.isDir()){//是一个文件夹
             strcat(pDestPath,"/");
             strcat(pDestPath,caFileName);
             bool ret=QFile::rename(pSrcPath,pDestPath);
             if(ret){
                 strcpy(respdu->caData,MOVE_FILE_OK);
             }else {
                  strcpy(respdu->caData,COMMON_ERR);
             }
          }else if (fileInfo.isFile()) {//一个文件
                strcpy(respdu->caData,MOVE_FILE_FAILURED);
        }
        write((char*)respdu, respdu->uiPDULen);
        free(respdu);
        respdu = NULL;
        break;
    }
    default:
        break;
    }
    //释放操作
    free(pdu);
    pdu=NULL;
    }else{
        PDU *respdu=NULL;
        respdu=mkPDU(0);
        respdu->uiMsgType=ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND;//回复
        //接收数据
        QByteArray buff=readAll();
        //把数据写入文件
        m_file.write(buff);
        m_iRecved+=buff.size();
        if(m_iTotal==m_iRecved){//表示接受数据完毕，可以关闭文件
            m_file.close();
            m_bUpload=false;//关闭传送

            strcpy(respdu->caData,UPLOAD_FILE_OK);
            //发送
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }else if(m_iTotal==m_iRecved){
            m_file.close();
            m_bUpload=false;//关闭传送

            strcpy(respdu->caData,UPLOAD_FILE_FAILURED);
            //发送
            write((char *)respdu,respdu->uiPDULen);
            free(respdu);
            respdu=NULL;
        }

    }
}
//处理客户端下线
void MyTcpSocket::clientOfflinet()
{
    //将在线状态设置为非在线状态
    OpeDB::getInstance().handleOffline(m_strName.toStdString().c_str());//获得char *
    //删除链表中的socket
    emit offline(this);//发送信号

}
//发送数据给客户端
void MyTcpSocket::sendFileToClient()
{
    char * pData=new char[4096];
    qint64 ret=0;
    while(true){
        //在用到read函数的地方使用一个前置判断！
        if(!m_file.isReadable())return;
        ret=m_file.read(pData,4096);
        if(ret>0 && ret<=4096){//读取到数据
            write(pData,ret);
        }else if(ret==0){
            m_file.close();
            break;
        }else if(ret<0){
            qDebug()<<"发送内容过程失败";
            m_file.close();
            break;
        }
    }
    delete []pData;
    pData=NULL;
}
