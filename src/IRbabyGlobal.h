/*
 * @Author: Caffreyfans
 * @Date: 2021-07-06 20:59:02
 * @LastEditTime: 2021-07-12 23:04:51
 * @Description: 
 */
#ifndef IRBABY_GLOBAL_H // 防止重复包含头文件
#define IRBABY_GLOBAL_H

#include <ArduinoJson.h> // 引入 ArduinoJson 库
#include <WiFiManager.h> // 引入 WiFiManager 库
#include <WiFiClient.h>  // 引入 WiFiClient 库
#ifdef USE_LED // 如果定义了 USE_LED
#include "jled.h" // 引入 jled 库
#endif // USE_LED

/* 可用的 JSON 变量 */
extern StaticJsonDocument<1024> recv_msg_doc; // 接收消息的 JSON 文档
extern StaticJsonDocument<1024> send_msg_doc; // 发送消息的 JSON 文档
extern StaticJsonDocument<1024> udp_msg_doc; // UDP 消息的 JSON 文档
extern StaticJsonDocument<1024> mqtt_msg_doc; // MQTT 消息的 JSON 文档

extern WiFiManager wifi_manager; // WiFi 管理器
extern WiFiClient wifi_client; // WiFi 客户端

extern uint8_t ir_send_pin; // 红外发送引脚
extern uint8_t ir_receive_pin; // 红外接收引脚
#ifdef USE_RF // 如果定义了 USE_RF
extern uint8_t rf315_send_pin; // RF 315 发送引脚
extern uint8_t rf315_receive_pin; // RF 315 接收引脚
extern uint8_t rf433_send_pin; // RF 433 发送引脚
extern uint8_t rf433_receive_pin; // RF 433 接收引脚
#endif

#ifdef USE_LED // 如果定义了 USE_LED
extern JLed led; // LED 对象
#endif // USE_LED

#endif // IRBABY_GLOBAL_H // 结束头文件保护