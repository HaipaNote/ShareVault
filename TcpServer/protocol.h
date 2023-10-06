#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
typedef unsigned int uint;

//注册消息提示
#define REGIST_OK "regist ok"
#define REGIST_FAILED "regist failed ： name exited"

//登录消息提示
#define LOGIN_OK "LOGIN ok"
#define LOGIN_FAILED "login failed ： name or pwd is error"


//查找用户
#define SEARCH_USR_NO "no search people"
#define SEARCH_USR_ONLINE "search people online"
#define SEARCH_USR_OFFLINE "search people offline"

//通用类型定义
#define UNKNOW_ERROR "unknow error"
#define EXISTED_FRIEND "existed friend"
#define ADD_FRIEND_OFFLINE "add friend offonline"
#define ADD_FRIEND_NO_EXIST "usr no exist"

//删除好友
#define DEL_FRIEND_OK "delete friend ok"

//文件夹不存在
#define DIR_NO_EXIST "cur dir not exist"

//文件名已经存在
#define FILE_NAME_EXIST "file name exist"

//创建成功
#define CREATE_DIR_OK "create dir ok"

//删除文件夹操作
#define DEL_DIR_OK "delete dir ok"
#define DEL_DIR_FAILURED "delete dir failured:is reguler file"

//删除文件操作
#define DEL_FILE_OK "delete FILE ok"
#define DEL_FILE_FAILURED "delete FILE failured:is reguler dir"


//重命名
#define RENAME_FILE_OK "rename file ok"
#define RENAME_FILE_FAILURED "rename dir failured:is reguler file"

//进入文件夹
#define ENTER_DIR_FAILURED "enter dir failured:is reguler file"

//上传文件
#define UPLOAD_FILE_OK "upload file ok"
#define UPLOAD_FILE_FAILURED "upload file failured"

//移动文件
#define MOVE_FILE_OK "move file ok"
#define MOVE_FILE_FAILURED "move file failured"

//系统问题
#define COMMON_ERR "operater failed :system is busy"

//消息类型：枚举
enum ENUM_MSG_TYPE{
    ENUM_MSG_TYPE_MIN=0,
    /*--------------注册-------------*/
    ENUM_MSG_TYPE_REGIST_REQUEST,//请求数据
    ENUM_MSG_TYPE_REGIST_RESPOND,//相应数据

    /*--------------登录-------------*/
    ENUM_MSG_TYPE_LOGIN_REQUEST,//请求数据
    ENUM_MSG_TYPE_LOGIN_RESPOND,//相应数据

    /*--------------在线用户-------------*/
    ENUM_MSG_TYPE_ALL_ONLINE_REQUEST,//请求数据
    ENUM_MSG_TYPE_ALL_ONLINE_RESPOND,//相应数据

    /*--------------搜索用户-------------*/
    ENUM_MSG_TYPE_SEARCH_USR_REQUEST,//请求数据
    ENUM_MSG_TYPE_SEARCH_USR_RESPOND,//相应数据

    /*--------------加好友-------------*/
    ENUM_MSG_TYPE_ADD_FRIEND_REQUEST,//请求数据
    ENUM_MSG_TYPE_ADD_FRIEND_RESPOND,//相应数据

    /*--------------同意与否加好友-------------*/
    ENUM_MSG_TYPE_ADD_FRIEND_AGREE,//同意添加
    ENUM_MSG_TYPE_ADD_FRIEND_REFUSE,//拒绝添加

    /*--------------刷新好友-------------*/
    ENUM_MSG_TYPE_FLUSH_FRIEND_REQUEST,//请求数据
    ENUM_MSG_TYPE_FLUSH_FRIEND_RESPOND,//相应数据

    /*--------------删除好友-------------*/
    ENUM_MSG_TYPE_DELETE_FRIEND_REQUEST,//请求数据
    ENUM_MSG_TYPE_DELETE_FRIEND_RESPOND,//相应数据

    /*--------------私聊-------------*/
    ENUM_MSG_TYPE_PRIVATE_CHAT_REQUEST,//请求数据
    ENUM_MSG_TYPE_PRIVATE_CHAT_RESPOND,//相应数据

    /*--------------群聊-------------*/
    ENUM_MSG_TYPE_GROUP_CHAT_REQUEST,//请求数据
    ENUM_MSG_TYPE_GROUP_CHAT_RESPOND,//相应数据

    /*--------------创建路径请求-------------*/
    ENUM_MSG_TYPE_CREATE_DIR_REQUEST,//请求数据
    ENUM_MSG_TYPE_CREATE_DIR_RESPOND,//相应数据

    /*--------------刷新文件-------------*/
    ENUM_MSG_TYPE_FLUSH_FILE_REQUEST,//请求数据
    ENUM_MSG_TYPE_FLUSH_FILE_RESPOND,//相应数据

    /*--------------删除目录请求-------------*/
    ENUM_MSG_TYPE_DEL_DIR_REQUEST,//请求数据
    ENUM_MSG_TYPE_DEL_DIR_RESPOND,//相应数据

    /*--------------重命名-------------*/
    ENUM_MSG_TYPE_RENAME_FILE_REQUEST,//请求数据
    ENUM_MSG_TYPE_RENAME_FILE_RESPOND,//相应数据

    /*--------------进入文件夹-------------*/
    ENUM_MSG_TYPE_ENTER_DIR_REQUEST,//请求数据
    ENUM_MSG_TYPE_ENTER_DIR_RESPOND,//相应数据

    /*--------------上传文件-------------*/
    ENUM_MSG_TYPE_UPLOAD_FILE_REQUEST,//请求数据
    ENUM_MSG_TYPE_UPLOAD_FILE_RESPOND,//相应数据

    /*--------------删除文件-------------*/
    ENUM_MSG_TYPE_DEL_FILE_REQUEST,//请求数据
    ENUM_MSG_TYPE_DEL_FILE_RESPOND,//相应数据

    /*--------------下载文件-------------*/
    ENUM_MSG_TYPE_DOWNLOAD_FILE_REQUEST,//请求数据
    ENUM_MSG_TYPE_DOWNLOAD_FILE_RESPOND,//相应数据

    /*--------------共享文件-------------*/
    ENUM_MSG_TYPE_SHARE_FILE_REQUEST,//请求数据
    ENUM_MSG_TYPE_SHARE_FILE_RESPOND,//相应数据

    /*--------------共享文件通知-------------*/
    ENUM_MSG_TYPE_SHARE_FILE_NOTE,
    ENUM_MSG_TYPE_SHARE_FILE_NOTE_RESPOND,

    /*--------------移动文件-------------*/
    ENUM_MSG_TYPE_MOVE_FILE_REQUEST,
    ENUM_MSG_TYPE_MOVE_FILE_RESPOND,

    ENUM_MSG_TYPE_MAX=0x00ffffff//32位数据
};
//文件信息
struct FileInfo{
    char caFileName[32];//文件名
    int iFileType;//文件类型

};



//协议
struct PDU{
    uint uiPDULen;//总的消息协议大小
    uint uiMsgType;//消息类型
    char caData[64];//指定消息
    uint uiMsgLen;//实际消息长度
    uint caMsg[];//实际消息，通过空的数组动态访问空间
};
//创建结构体
PDU *mkPDU(uint uiMsgLen);

#endif // PROTOCOL_H
