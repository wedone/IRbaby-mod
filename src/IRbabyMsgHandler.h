src/IRbabyMsgHandler.h
#ifndef IREASY_MSG_HANDLE_H  // 头文件保护宏开始
#define IREASY_MSG_HANDLE_H

#include <ArduinoJson.h>  // 引入 ArduinoJson 库

// 定义消息类型的枚举
typedef enum msgtype
{
    mqtt,  // MQTT 消息类型
    udp    // UDP 消息类型
} MsgType;  // 消息类型的别名

// 函数声明：处理消息
bool msgHandle(StaticJsonDocument<1024> *p_recv_msg_doc, MsgType msg_type);  // 接收 JSON 文档和消息类型作为参数

#endif  // 头文件保护宏结束