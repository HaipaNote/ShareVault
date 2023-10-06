#ifndef OPEDB_H
#define OPEDB_H

#include <QObject>
#include <QSqlDatabase>//连接数据库
#include <QSqlQuery>//查询数据库
#include <QStringList>//返回在线人名列表
class OpeDB : public QObject
{
    Q_OBJECT
public:
    explicit OpeDB(QObject *parent = nullptr);
    static OpeDB& getInstance();//局部成员对象，（实现单例模式）
    void init();//连接实现初始化
    bool handleRegist(const char * name,const char * pwd);//注册
    bool handleLogin(const char * name,const char * pwd);//登录
    void handleOffline(const char * name);//处理下线操作
    QStringList handleAllOnline();//找到所有在线人数
    int handleSearchUsr(const char * name);//查找用户
    int  handleAddFriend(const char * prename,const char * name);//添加好友:prename:好友，name:自己
    QStringList handleFlushFriend(const char * name);//刷新好友列表
    bool handleDelFriend(const char * name,const char * friendName);//删除好友
    void handleAgreeAddFriend(const char *pername, const char *name);//同意添加好友
    ~OpeDB();
signals:

public slots:
private:
    QSqlDatabase m_db;//连接数据库

};

#endif // OPEDB_H
