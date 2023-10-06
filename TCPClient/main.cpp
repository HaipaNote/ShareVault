#include "tcpclient.h"
#include <QApplication>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    TcpClient w;
//    w.show();

     TcpClient::getInstance().show();//通过引用获得当前界面对象

    return a.exec();
}
