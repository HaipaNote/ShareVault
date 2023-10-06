#ifndef OPEWIDGET_H
#define OPEWIDGET_H

#include <QWidget>
#include <QListWidget>
#include "friend.h"
#include "book.h"

#include <QStackedWidget>

class OpeWidget : public QWidget
{
    Q_OBJECT
public:
    explicit OpeWidget(QWidget *parent = nullptr);
    static OpeWidget &getInstance();//使用static实现单利模式，可以通过类名直接调用
    Friend *getFriend();//获得好友的操作界面
    Book * getBook();//获取book操作界面
signals:

public slots:
private:
    QListWidget * m_pListW;//列表对象
    Friend * m_pFriend;//好友对象
    Book * m_pBook;//图书对象
    QStackedWidget * m_pSw;//栈对象，每次只显示一个

    QListWidgetItem *usernameItem;//用户名编辑



};
#endif // OPEWIDGET_H
