#ifndef IRBABYIR_H // 头文件保护宏
#define IRBABYIR_H

#include <Arduino.h> // 引入 Arduino 库
#include <IRsend.h>  // 引入 IR 发送库
#include <IRrecv.h>  // 引入 IR 接收库
#include "../lib/Irext/include/ir_decode.h" // 引入 IR 解码库

// 声明函数：加载 IR 引脚
void loadIRPin(uint8_t send_pin, uint8_t recv_pin);
// 声明函数：启用 IR
void enableIR();
// 声明函数：禁用 IR
void disableIR();
// 声明函数：发送状态
void sendStatus(String file_name, t_remote_ac_status status);
// 声明函数：发送按键
bool sendKey(String file_name, int key);
// 声明函数：发送 IR
bool sendIR(String file_name);
// 声明函数：接收 IR
void recvIR();
// 声明函数：保存 IR
bool saveIR(String file_name);
// 声明函数：初始化空调
void initAC(String);
#endif // IRBABAYIR_H // 结束头文件保护宏