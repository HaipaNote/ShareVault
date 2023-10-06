#ifndef PRIVATECHATE_H
#define PRIVATECHATE_H

#include <QWidget>
#include "protocol.h"
namespace Ui {
class PrivateChate;
}

class PrivateChate : public QWidget
{
    Q_OBJECT

public:
    explicit PrivateChate(QWidget *parent = nullptr);
    ~PrivateChate();
    static PrivateChate &getInstance();//设置为单例模式
    void setChatName(QString strName);//设置聊天对象
    void updateMsg(const PDU * pdu);//更新显示聊天消息

private slots:
    void on_sendMsg_pb_clicked();

private:
    Ui::PrivateChate *ui;
    QString m_strChatName;//保存聊天对象名
    QString m_strLoginName;//保存自己的名字
};

#endif // PRIVATECHATE_H
