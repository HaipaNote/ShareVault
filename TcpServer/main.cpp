#include "tcpserver.h"
#include <QApplication>

#include "opedb.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpServer w;
    w.show();

    //直接初始化数据库
    OpeDB::getInstance().init();

    return a.exec();
}
