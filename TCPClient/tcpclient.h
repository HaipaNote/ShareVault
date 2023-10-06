#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QWidget>

#include <QFile>
#include <QTcpSocket>
#include "protocol.h"
#include "opewidget.h"
namespace Ui {
class TcpClient;
}

class TcpClient : public QWidget
{
    Q_OBJECT

public:
    explicit TcpClient(QWidget *parent = nullptr);
    ~TcpClient();
    void loadConfig();//加载配置
    //将登录写成单例模式，方便后面调用
    static TcpClient &getInstance();
    //获取socket，不论在哪里调用都会使用socket
    QTcpSocket &getTcpSocket();
    //通过共有方法，获得m_strLoginName
    QString loginName();
    //返回当前路径信息
    QString curPath();
    //设置当前路径
    void setCurPath(QString strCurPath);

public slots:
     //捕获连接信号
    void showConnect();
    //接收服务器返回的注册消息
    void recvMsg();

private slots:
  //  void on_pushButton_clicked();

    void on_login_pb_clicked();

    void on_regist_pb_clicked();

    void on_cancel_pb_clicked();

private:
    Ui::TcpClient *ui;
    QString m_strIP;//存放ip地址
    quint16 m_strPort;//无符号短整型，存放端口号
    QTcpSocket m_tcpSocket;//socket对象，连接服务器，与服务器数据交互
    QString m_strLoginName;//临时保存name
    QString m_strCurPath;//记录当前路径
    QFile m_file;//解决局部变量释放
};

#endif // TCPCLIENT_H
