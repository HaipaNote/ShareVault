#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QWidget>
#include <QFile>
#include <QTcpSocket>
#include "mytcpserver.h"

namespace Ui {
class TcpServer;
}

class TcpServer : public QWidget
{
    Q_OBJECT

public:
    explicit TcpServer(QWidget *parent = nullptr);
    ~TcpServer();
    void loadConfig();

private:
    Ui::TcpServer *ui;
    QString m_strIP;//存放ip地址
    quint16 m_strPort;//无符号短整型，存放端口号
};

#endif // TCPSERVER_H
