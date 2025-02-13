src/IRbabyGlobal.cpp
/*
 * @Author: Caffreyfans
 * @Date: 2021-07-06 20:59:02
 * @LastEditTime: 2021-07-12 23:05:04
 * @Description: 
 */
#include "IRbabyGlobal.h" // 包含头文件 IRbabyGlobal.h
#include "defines.h"      // 包含定义文件 defines.h

StaticJsonDocument<1024> recv_msg_doc; // 创建接收消息的静态 JSON 文档
StaticJsonDocument<1024> send_msg_doc; // 创建发送消息的静态 JSON 文档
StaticJsonDocument<1024> udp_msg_doc;  // 创建 UDP 消息的静态 JSON 文档
StaticJsonDocument<1024> mqtt_msg_doc; // 创建 MQTT 消息的静态 JSON 文档

WiFiManager wifi_manager; // 创建 WiFi 管理器实例
WiFiClient wifi_client;   // 创建 WiFi 客户端实例

uint8_t ir_send_pin = T_IR; // 定义红外发送引脚
uint8_t ir_receive_pin = R_IR; // 定义红外接收引脚

#ifdef USE_RF // 如果定义了 USE_RF
uint8_t rf315_send_pin = T_315; // 定义 RF 315 发送引脚
uint8_t rf315_receive_pin = R_315; // 定义 RF 315 接收引脚
uint8_t rf433_send_pin = T_433; // 定义 RF 433 发送引脚
uint8_t rf433_receive_pin = R_433; // 定义 RF 433 接收引脚
#endif

#ifdef USE_LED // 如果定义了 USE_LED
JLed led = JLed(LED_PIN); // 创建 LED 实例
#endif // USE_LED