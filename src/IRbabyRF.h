#ifndef IRBABYRF_H  // 防止重复包含
#define IRBABYRF_H
#include <Arduino.h>  // 包含 Arduino 库
#include "RCSwitch.h"  // 包含 RCSwitch 库

typedef enum  // 定义枚举类型
{
    RF315,  // 315MHz RF
    RF433   // 433MHz RF
} RFTYPE;  // 枚举类型名称为 RFTYPE

void initRF(void);  // 初始化 RF 的函数声明
bool sendRFFile(String file_name);  // 发送 RF 文件的函数声明
void sendRFData(unsigned long code, unsigned int length, RFTYPE type);  // 发送 RF 数据的函数声明
void recvRF(void);  // 接收 RF 的函数声明
void disableRF(void);  // 禁用 RF 的函数声明
void enableRF(void);  // 启用 RF 的函数声明
bool saveRF(String file_name);  // 保存 RF 文件的函数声明

extern RCSwitch rf315;  // 声明外部变量 rf315
extern RCSwitch rf433;  // 声明外部变量 rf433
#endif  // 结束防止重复包含