#include "IRbabyMQTT.h" // 包含 IRbabyMQTT 头文件
#include "PubSubClient.h" // 包含 PubSubClient 头文件
#include "IRbabySerial.h" // 包含 IRbabySerial 头文件
#include "ArduinoJson.h" // 包含 ArduinoJson 头文件
#include "IRbabyUserSettings.h" // 包含 IRbabyUserSettings 头文件
#include "IRbabyMsgHandler.h" // 包含 IRbabyMsgHandler 头文件
#include "IRbabyGlobal.h" // 包含 IRbabyGlobal 头文件

PubSubClient mqtt_client(wifi_client); // 创建一个 MQTT 客户端实例

void callback(char *topic, byte *payload, unsigned int length); // 声明回调函数

void mqttInit() // 初始化 MQTT
{
    INFOLN("MQTT Init"); // 输出初始化信息
    mqttReconnect(); // 尝试重新连接 MQTT
    mqtt_client.setCallback(callback); // 设置回调函数
}

/**************************************
 * 订阅示例: /IRbaby/chip_id/set/#
 **************************************/
bool mqttReconnect() // 重新连接 MQTT
{
    bool flag = false; // 连接标志
    if (ConfigData.containsKey("mqtt")) // 检查配置中是否包含 mqtt
    {
        JsonObject mqtt_obj = ConfigData["mqtt"]; // 获取 mqtt 配置对象
        const char *host = mqtt_obj["host"]; // 获取主机地址
        int port = mqtt_obj["port"]; // 获取端口
        const char *user = mqtt_obj["user"]; // 获取用户名
        const char *password = mqtt_obj["password"]; // 获取密码
        if (host && port) // 如果主机和端口有效
        {
            mqtt_client.setServer(host, port); // 设置 MQTT 服务器
            String chip_id = String(ESP.getChipId(), HEX); // 获取芯片 ID
            chip_id.toUpperCase(); // 转换为大写
            DEBUGF("Trying to connect %s:%d\n", host, port); // 输出连接信息
            if (mqtt_client.connect(chip_id.c_str(), user, password)) // 尝试连接
            {
                String sub_topic = String("/IRbaby/") + chip_id + String("/send/#"); // 订阅主题
                DEBUGF("MQTT subscribe %s\n", sub_topic.c_str()); // 输出订阅信息
                mqtt_client.subscribe(sub_topic.c_str()); // 订阅主题
                flag = true; // 设置连接标志为 true
            }
        }
        INFOF("MQTT state rc = %d\n", mqtt_client.state()); // 输出 MQTT 状态
    }
    delay(1000); // 延迟 1 秒
    return flag; // 返回连接标志
}

void mqttDisconnect() // 断开 MQTT 连接
{
    mqtt_client.disconnect(); // 断开连接
}

void callback(char *topic, byte *payload, unsigned int length) // 回调函数
{
    mqtt_msg_doc.clear(); // 清空消息文档

    String payload_str = ""; // 初始化负载字符串
    for (uint32_t i = 0; i < length; i++) // 遍历负载
        payload_str += (char)payload[i]; // 将负载转换为字符串
    String topic_str(topic); // 将主题转换为字符串
    uint8_t index = 0; // 初始化索引
    String option; // 初始化选项字符串
    String func; // 初始化函数字符串
    do
    {
        int divsion = topic_str.lastIndexOf("/"); // 查找最后一个斜杠的位置
        String tmp = topic_str.substring(divsion + 1, -1); // 获取主题的最后一部分
        topic_str = topic_str.substring(0, divsion); // 更新主题字符串
        switch (index++) // 根据索引处理不同的主题部分
        {
        case 0:
            func = tmp; // 第一个部分为函数
            break;
        case 1:
            mqtt_msg_doc["params"]["file"] = tmp; // 第二个部分为文件
            break;
        case 2:
            mqtt_msg_doc["params"]["type"] = tmp; // 第三个部分为类型
            option = tmp; // 保存选项
            break;
        case 3:
            mqtt_msg_doc["params"]["signal"] = tmp; // 第四个部分为信号
            break;
        case 4:
            mqtt_msg_doc["cmd"] = tmp; // 第五个部分为命令
        default:
            break; // 默认情况
        }
    } while (topic_str.lastIndexOf("/") > 0); // 继续处理直到没有斜杠
    mqtt_msg_doc["params"][option][func] = payload_str; // 将负载字符串存入消息文档
    msgHandle(&mqtt_msg_doc, MsgType::mqtt); // 处理消息
}

bool mqttConnected() // 检查是否连接
{
    return mqtt_client.connected(); // 返回连接状态
}

void mqttLoop() // MQTT 循环
{
    mqtt_client.loop(); // 处理 MQTT 客户端循环
}

void mqttPublish(String topic, String payload) // 发布消息
{
    mqtt_client.publish(topic.c_str(), payload.c_str()); // 发布主题和负载
}

void mqttPublishRetained(String topic, String payload) // 发布保留消息
{
    mqtt_client.publish(topic.c_str(), payload.c_str(), true); // 发布保留消息
}

void mqttCheck() // 检查 MQTT 连接
{
    if (!mqttConnected()) // 如果未连接
    {
        DEBUGLN("MQTT disconnect, try to reconnect"); // 输出断开信息
        mqtt_client.disconnect(); // 断开连接
        mqttReconnect(); // 尝试重新连接
#ifdef USE_LED
        led.Blink(500, 500); // LED 闪烁
#endif // USE_LED
    } else {
#ifdef USE_LED
        led.On(); // LED 开启
#endif // USE_LED
    }
}