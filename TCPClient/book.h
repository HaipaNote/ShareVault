#ifndef BOOK_H
#define BOOK_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "protocol.h"
#include <QTimer>

class Book : public QWidget
{
    Q_OBJECT
public:
    explicit Book(QWidget *parent = nullptr);
    void updateFileList(const PDU *pdu);//刷新文件信息列表
    void clearEnterDir();//清空进入文件夹
    QString enterDir();//获取当前文件

    void setDownLoadStatus(bool status);//设置下载状态
    bool getDownloadStatus();//获取当前私有下载状态
    QString getSaveFilePath();//获取当前私有文件路径
    QString getShareFileName();//获得分享私有文件


    qint64 m_iTotal;//下载文件总的大小
    qint64 m_iRecved;//已经接收大小

signals:

public slots:
    void createDir();//创建文件夹操作
    void flushFile();//刷新文件夹
    void delDir();//删除文件夹
    void renameFile();//重命名文件
    void enterDir(const QModelIndex & idnex);//进入文件夹
    void returnPre();//返回上一级目录
    void uploadFile();//上传文件
    void uploadFileDate();//上传文件的定时器处理
    void delRegFile();//删除文件
    void downloadFile();//下载文件
    void shareFile();//共享文件
    void moveFile();//移动文件
    void selectDestDir();//选择目的目录

private:
    QListWidget * m_pBoookListW;//显示文件列表
    QPushButton * m_pReturnPB;//返回
    QPushButton * m_pCreateDirPB;//创建文件夹
    QPushButton * m_pDelDriPB;//删除文件夹
    QPushButton * m_pRnamePB;//重命名文件夹
    QPushButton * m_pFlushFilePB;//刷新文件夹

    QPushButton * m_pUploadPB;//上传文件
    QPushButton * m_pDelFilePB;//删除文件
    QPushButton * m_pDownloadPB;//下载文件
    QPushButton * m_pShareFilePB;//分享文件
    QPushButton * m_pMoveFilePB;//移动文件
    QPushButton * m_pSelectDirPB;//选择移动到那个文件夹

    QString m_strEnterDir;//进入文件夹路径
    QString m_strUploadFilePath;//用来保存上传文件的路径

    QTimer * m_pTimer;//使用定时器分离两部分内容：一部分是发送的文件名大小，一部分是内部的内容

    QString m_strSaveFilePath;//保存文件路径
    bool m_bDownload;//是否处于下载状态
    QString m_strShareFileName;//分享文件
    QString m_strMoveFileName;//移动文件
    QString m_strMoveFilePath;//移动文件路径
    QString m_strDestDir;//目的路径
};

#endif // BOOK_H
