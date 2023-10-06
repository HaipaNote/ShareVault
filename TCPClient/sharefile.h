#ifndef SHOWFILE_H
#define SHOWFILE_H

#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QScrollArea>//浏览区域
#include <QCheckBox>
#include <QListWidget>

#include "opewidget.h"
#include "tcpclient.h"
class ShareFile : public QWidget
{
    Q_OBJECT
public:
    explicit ShareFile(QWidget *parent = nullptr);
    static ShareFile &getInstance();//设置为单例模式
    void updateFriend(QListWidget * pFriendList);//更新好友列表
signals:

public slots:
       void cancelSelect();//取消选择
       void selectAll();//全选

       void okShare();//确定
       void cancelShare();//取消
private:
    QPushButton * m_pSelectAllPB;//全选按钮
    QPushButton * m_pCancelSelectPB;//取消全选按钮

    QPushButton * m_pOKPB;//确定
    QPushButton * m_pCancelPB;//取消按钮
    QScrollArea *m_pSA;//展示区域
    QWidget * m_pFriendW;//显示好友
    QButtonGroup * m_pButtonGroup;//管理全部好友

    QVBoxLayout *m_pFriendWVBL;//好友列表垂直布局


};

#endif // SHOWFILE_H
