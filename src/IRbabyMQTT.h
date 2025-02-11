#ifndef IRBABY_MQTT_H // 头文件保护宏开始
#define IRBABY_MQTT_H // 头文件保护宏结束
#include <WString.h> // 引入 WString 头文件

/* MQTT 初始化 */
void mqttInit(); // 初始化 MQTT 函数声明

/* MQTT 重连 */
bool mqttReconnect(); // MQTT 重连函数声明，返回布尔值

/* MQTT 断开连接 */
void mqttDisconnect(); // 断开 MQTT 连接的函数声明

/* MQTT 请求连接 */
bool mqttConnected(); // 检查 MQTT 是否连接的函数声明，返回布尔值

/* MQTT 接收循环 */
void mqttLoop(); // MQTT 接收循环的函数声明

/* MQTT 信息发送 */
void mqttPublish(String topic, String payload); // 发送 MQTT 信息的函数声明

/* MQTT 信息发送 */
void mqttPublishRetained(String topic, String payload); // 发送保留的 MQTT 信息的函数声明

/* MQTT 连接检查 */
void mqttCheck(); // 检查 MQTT 连接状态的函数声明

#endif // IRBABY_MQTT_H // 头文件保护宏结束