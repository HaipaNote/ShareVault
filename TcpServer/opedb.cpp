#include "opedb.h"

#include <QMessageBox>//数据信息打印
#include <QDebug>
OpeDB::OpeDB(QObject *parent) : QObject(parent)
{
    m_db=QSqlDatabase::addDatabase("QSQLITE");//指明操作的数据库是sqlite

}
//静态成员函数
OpeDB &OpeDB::getInstance()
{
    static OpeDB instance;
    return instance;
}
//连接数据库
void OpeDB::init()
{
    //连接数据库
    m_db.setHostName("localhost");
    m_db.setDatabaseName("D:\\Qt\\QT-object\\TcpServer\\cloud.db");
    if( m_db.open()){
       //查询一下数据
        QSqlQuery query;
        query.exec("select * from usrInfo");
        while(query.next()){
            QString data=QString("%1 %2 %3").arg(query.value(0).toString()).arg(query.value(1).toString()).arg(query.value(2).toString());
           qDebug()<<data;
        }


    }else{
        QMessageBox::critical(NULL, "db", "open db error");
    }
}

//注册处理
bool OpeDB::handleRegist(const char *name, const char *pwd)
{
    //拼接语句
//    char caQuery[128]={'\0'};
//    sprintf(caQuery,"")
    if(name ==NULL || pwd==NULL){
        return false;
    }
    //插入操作
    QString data=QString("insert into usrinfo(name,pwd) values(\'%1\',\'%2\')").arg(name).arg(pwd);
    qDebug()<<data;
    QSqlQuery query;
    return query.exec(data);
}
//登录处理
bool OpeDB::handleLogin(const char *name, const char *pwd)
{
    if(name ==NULL || pwd==NULL){
        return false;
    }
    //查询操作
    QString data=QString("select * from  usrInfo  where name=\'%1\' and pwd=\'%2\' and online=0").arg(name).arg(pwd);
    qDebug()<<"select * from usrInfo:"<<data;
    QSqlQuery query;
    query.exec(data);
    //return query.next();//逐行获取数据，获得数据后，数据被放到query对象中
    //登录成功之后需要将online的状态至为1
    if(query.next()){
        data=QString("update  usrInfo set online=1  where name=\'%1\' and pwd=\'%2\'").arg(name).arg(pwd);
        qDebug()<<"update  usrInfo set online=1 "<<data;
        QSqlQuery query;
        query.exec(data);
        return true;
    }else{
        return false;
    }
}
//处理下线操作
void OpeDB::handleOffline(const char *name)
{
    if(name ==NULL ){
        return ;
    }
    QString data=QString("update usrInfo set online=0 where name=\'%1\'").arg(name);
    qDebug()<<"update usrInfo set online=0"<<data;
    QSqlQuery query;
    query.exec(data);
}
//找到所有在线人数
QStringList OpeDB::handleAllOnline()
{

    QString data=QString("select name from usrInfo where online=1");

    QSqlQuery query;
    query.exec(data);

    QStringList result;
    result.clear();

    //循环调用next(),不断获取
    while(query.next()){
        result.append(query.value(0).toString());
    }
    qDebug()<<"result:"<<result;
    return result;
}
//查找用户
int OpeDB::handleSearchUsr(const char *name)
{
    if(name==NULL){
        return  -1;
    }
     QString data=QString("select online from usrInfo where name=\'%1\'").arg(name);
     QSqlQuery query;
     query.exec(data);
     if(query.next()){
         int ret=query.value(0).toInt();//转换成int类型
         if(ret==1){
             return 1;//表示在线状态
         }else if(ret==0){
             return 0;//表示离线状态
         }
     }else {
        return -1;//不存在这个人
     }
}
//添加好友
int OpeDB::handleAddFriend(const char *prename, const char *name)
{
    if(prename==NULL || name==NULL){
        return -1;
    }
    //自己在查找别人名字，先会查找该人是否存在，不论是哪一方都需要进行查询，
    //两边都要查询，所以需要做两次查询
    //连表查询
    QString data = QString("select * from friend where (id=(select id from usrInfo where name=\'%1\') and friendId = (select id from usrInfo where name=\'%2\')) "
                              "or (id=(select id from usrInfo where name=\'%3\') and friendId = (select id from usrInfo where name=\'%4\'))").arg(prename).arg(name).arg(name).arg(prename);
    qDebug()<<"select * from friendId"<<data;
    QSqlQuery query;
    query.exec(data);
    if(query.next()){
        return 0;//双方已经是好友
    }else {
       //不是好友，先判断是否在线
        data=QString("select online from usrInfo where name=\'%1\'").arg(prename);
             QSqlQuery query;
             query.exec(data);
             if(query.next()){
                 int ret=query.value(0).toInt();//转换成int类型
                 if(ret==1){
                     return 1;//表示在线状态
                 }else if(ret==0){
                     return 2;//表示离线状态
                 }
             }else {
                return 3;//不存在这个人
             }
    }
}
//刷新好友列表
QStringList OpeDB::handleFlushFriend(const char *name)
{
    //存放查到的朋友列表
    QStringList strFriendList;
    strFriendList.clear();
    if(name==NULL){
        return strFriendList;
    }
    //第一次人名作为friendId中的id时的查询
    QString data=QString("select name from usrInfo where id in (select id from friend where friendId=("
                         "select id from usrInfo where name=\'%1\')) and online=1").arg(name);
    qDebug()<<"strFriendList:"<<data;//test
    QSqlQuery query;
    query.exec(data);
    //拿到结果
    while(query.next()){
       strFriendList.append( query.value(0).toString());//添加到list中
       qDebug()<<"friend : name"<<query.value(0).toString();//test
    }


    //第二次人名作为friendId中的friendid时的查询
//     data=QString("select name from usrInfo where id in (select friendId from friend where id=("
//                  "select id from usrInfo where name=\'%1\')) and online=1").arg(query.value(0).toString());
    data=QString("select name from usrInfo where id in (select friendId from friend where id=("
                 "select id from usrInfo where name=\'%1\')) and online=1").arg(name);


     query.exec(data);
     //拿到结果
     while(query.next()){
         strFriendList.append(query.value(0).toString());//添加到lsit中
        qDebug()<<"friend : name"<<query.value(0).toString();//test
     }
     return strFriendList;

}
//删除好友
bool OpeDB::handleDelFriend(const char *name, const char *friendName)
{
    if(name ==NULL || friendName ==NULL){
        return  false;
    }
    //删除好友
    QString data=QString("delete from friend where id=(select id from usrInfo  where name=\'%1\')"
                         "and friendId=(select id from usrInfo where name=\'%2\')").arg(name).arg(friendName);
    QSqlQuery query;
    query.exec(data);

    data=QString("delete from friend where id=(select id from usrInfo  where name=\'%1\')"
                             "and friendId=(select id from usrInfo where name=\'%2\')").arg(friendName).arg(name);
    query.exec(data);
    return true;
}
//同意添加好友
void OpeDB::handleAgreeAddFriend(const char *pername, const char *name)
{
    if (NULL == pername || NULL == name)
        {
            return;
        }
        QString data = QString("insert into friend(id, friendId) values((select id from usrInfo where name=\'%1\'), (select id from usrInfo where name=\'%2\'))").arg(pername).arg(name);
        QSqlQuery query;
        query.exec(data);
}

OpeDB::~OpeDB()
{
    //关闭数据库
    m_db.close();
}
