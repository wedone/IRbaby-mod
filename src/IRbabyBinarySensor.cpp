/*
 * @Author: Caffreyfans
 * @Date: 2021-07-12 23:18:11
 * @LastEditTime: 2021-07-12 23:45:29
 * @Description: 二进制传感器的实现文件
 */
#include "IRbabyBinarySensor.h"  // 包含二进制传感器的头文件

#include "IRbabyMQTT.h"          // 包含MQTT相关的头文件
#include "defines.h"             // 包含定义的常量和宏

// 初始化二进制传感器
void binary_sensor_init() { 
    pinMode(SENSOR_PIN, INPUT_PULLUP);  // 设置传感器引脚为输入并启用上拉电阻
}

// 主循环函数，持续检查传感器状态
void binary_sensor_loop() {
    if (mqttConnected()) {  // 检查MQTT连接是否成功
        String chip_id = String(ESP.getChipId(), HEX);  // 获取ESP芯片ID并转换为十六进制字符串
        chip_id.toUpperCase();  // 将芯片ID转换为大写
        int value = digitalRead(SENSOR_PIN);  // 读取传感器引脚的值
        String message = value == HIGH ? "on" : "off";  // 根据传感器值设置消息内容
        String topic = "/IRbaby/" + chip_id + "/sensor";  // 构建MQTT主题
        mqttPublish(topic, message);  // 发布消息到MQTT主题
    }
}