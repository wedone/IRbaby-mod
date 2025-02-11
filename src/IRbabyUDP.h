#ifndef IRBABY_UDP_H // 防止头文件重复包含
#define IRBABY_UDP_H // 定义头文件标识

#include "ArduinoJson.h" // 包含 ArduinoJson 库
#include "ESP8266WiFi.h" // 包含 ESP8266 WiFi 库

#define UDP_PORT 4210 // 定义 UDP 端口
#define UDP_PACKET_SIZE 255 // 定义 UDP 数据包大小

void udpInit(); // 声明初始化 UDP 的函数
char* udpRecive(); // 声明接收 UDP 数据的函数
uint32_t sendUDP(StaticJsonDocument<1024>* doc, IPAddress ip); // 声明发送 UDP 数据的函数
uint32_t returnUDP(StaticJsonDocument<1024>* doc); // 声明返回 UDP 数据的函数

extern IPAddress remote_ip; // 声明外部变量 remote_ip

#endif // IRBABY_UDP_H // 结束头文件保护