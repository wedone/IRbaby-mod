#include "IRbabyha.h"  // 引入IRbabyha头文件
#include "IRbabyGlobal.h"  // 引入IRbabyGlobal头文件
#include "IRbabyMQTT.h"  // 引入IRbabyMQTT头文件
#include "IRbabySerial.h"  // 引入IRbabySerial头文件

void returnACStatus(String filename, t_remote_ac_status ac_status)  // 定义返回空调状态的函数
{
    send_msg_doc.clear();  // 清空发送消息文档
    String chip_id = String(ESP.getChipId(), HEX);  // 获取ESP芯片ID并转换为十六进制字符串
    chip_id.toUpperCase();  // 将芯片ID转换为大写
    String topic_head = "/IRbaby/" + chip_id + "/state/" + filename + "/";  // 构建MQTT主题头
    const char* mode[] = {"cool", "heat", "auto", "fan_only", "dry"};  // 定义空调模式数组
    const char* fan[] = {"auto", "low", "medium", "high", "max"};  // 定义风速数组
    const char* swing[] = {"on","off"};  // 定义摆动状态数组
    if (ac_status.ac_power == AC_POWER_OFF)  // 如果空调电源关闭
        mqttPublish(topic_head + "mode", "off");  // 发布模式为“off”
    else
        mqttPublish(topic_head + "mode", mode[(int)ac_status.ac_mode]);  // 发布当前空调模式
    mqttPublish(topic_head + "temperature", String((int)ac_status.ac_temp + 16));  // 发布当前温度
    mqttPublish(topic_head + "fan", fan[(int)ac_status.ac_wind_speed]);  // 发布风速
    mqttPublish(topic_head + "swing", swing[(int)ac_status.ac_swing]);  // 发布摆动状态
}

// 自动注册当前空调
void registAC(String filename, bool flag)  // 定义注册空调的函数
{
    send_msg_doc.clear();  // 清空发送消息文档
    String chip_id = String(ESP.getChipId(), HEX);  // 获取ESP芯片ID并转换为十六进制字符串
    chip_id.toUpperCase();  // 将芯片ID转换为大写
    String reg_topic_head = "homeassistant/climate/IRbaby-"+ chip_id + "_climate_" + filename +"/config";  // 构建注册主题头
    String reg_content;  // 注册内容字符串
    if (flag) {  // 如果标志为真
        send_msg_doc["platform"] = "mqtt";  // 设置平台为MQTT
        send_msg_doc["name"] = filename;  // 设置空调名称

        JsonArray modes = send_msg_doc.createNestedArray("modes");  // 创建模式数组
        modes.add("auto");  // 添加自动模式
        modes.add("heat");  // 添加加热模式
        modes.add("cool");  // 添加制冷模式
        modes.add("fan_only");  // 添加仅风扇模式
        modes.add("dry");  // 添加干燥模式
        modes.add("off");  // 添加关闭模式

        JsonArray swing_modes = send_msg_doc.createNestedArray("swing_modes");  // 创建摆动模式数组
        swing_modes.add("on");  // 添加开启摆动
        swing_modes.add("off");  // 添加关闭摆动
        send_msg_doc["max_temp"] = 30;  // 设置最大温度
        send_msg_doc["min_temp"] = 16;  // 设置最小温度

        JsonArray fan_modes = send_msg_doc.createNestedArray("fan_modes");  // 创建风速模式数组
        fan_modes.add("auto");  // 添加自动风速
        fan_modes.add("low");  // 添加低风速
        fan_modes.add("medium");  // 添加中风速
        fan_modes.add("high");  // 添加高风速
        String cmd_head_topic = "/IRbaby/" + chip_id + "/send/ir/local/" + filename + "/";  // 构建命令主题头
        String state_head_topic = "/IRbaby/" + chip_id + "/state/" + filename + "/";  // 构建状态主题头
        send_msg_doc["mode_command_topic"] = cmd_head_topic + "mode";  // 设置模式命令主题
        send_msg_doc["mode_state_topic"] = state_head_topic + "mode";  // 设置模式状态主题
        send_msg_doc["temperature_command_topic"] = cmd_head_topic + "temperature";  // 设置温度命令主题
        send_msg_doc["temperature_state_topic"] = state_head_topic + "temperature";  // 设置温度状态主题
        send_msg_doc["fan_mode_command_topic"] = cmd_head_topic + "fan";  // 设置风速命令主题
        send_msg_doc["fan_mode_state_topic"] = state_head_topic + "fan";  // 设置风速状态主题
        send_msg_doc["swing_mode_command_topic"] = cmd_head_topic + "swing";  // 设置摆动命令主题
        send_msg_doc["swing_mode_state_topic"] = state_head_topic + "swing";  // 设置摆动状态主题
        send_msg_doc["precision"] = 0.1;  // 设置精度
        send_msg_doc["unique_id"] = "IRbaby-" + chip_id + "_climate_" + filename;  // 设置唯一ID

        JsonObject device = send_msg_doc.createNestedObject("device");  // 创建设备对象
        JsonArray device_identifiers = device.createNestedArray("identifiers");  // 创建设备标识符数组
        device_identifiers.add("IRbaby-" + chip_id);  // 添加设备标识符
        device["name"] = "IRbaby-" + chip_id;  // 设置设备名称
        device["manufacturer"] = "espressif";  // 设置制造商
        device["model"] = "ESP8266";  // 设置设备型号
        device["sw_version"] = "IRbaby " + String(FIRMWARE_VERSION);  // 设置软件版本
    }
    if (flag) {  // 如果标志为真
        serializeJson(send_msg_doc, reg_content);  // 序列化JSON文档
    }
    DEBUGLN(reg_topic_head);  // 打印注册主题头
    mqttPublishRetained(reg_topic_head, reg_content);  // 发布保留消息
}