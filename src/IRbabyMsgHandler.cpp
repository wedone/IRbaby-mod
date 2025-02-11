language:src/IRbabyMsgHandler.cpp
#include "IRbabyMsgHandler.h" // 包含消息处理头文件
#include "IRbabySerial.h" // 包含串口通信头文件
#include "IRbabyUserSettings.h" // 包含用户设置头文件
#include "IRbabyMQTT.h" // 包含MQTT头文件
#include "IRbabyUDP.h" // 包含UDP头文件
#include "ESP8266WiFi.h" // 包含ESP8266 WiFi头文件
#include "IRbabyIR.h" // 包含红外头文件
#include "../lib/Irext/include/ir_ac_control.h" // 包含空调控制头文件
#include "IRbabyOTA.h" // 包含OTA更新头文件
#include "defines.h" // 包含定义头文件
#include <LittleFS.h> // 包含LittleFS文件系统头文件
#include "IRbabyRF.h" // 包含RF头文件
#include "IRbabyGlobal.h" // 包含全局变量头文件
#include "IRbabyha.h" // 包含HA头文件

bool msgHandle(StaticJsonDocument<1024> *p_recv_msg_doc, MsgType msg_type) // 消息处理函数
{
    if (LOG_DEBUG) // 如果开启调试日志
    {
        serializeJsonPretty(*p_recv_msg_doc, Serial); // 将接收到的JSON文档序列化并打印
        Serial.println(); // 打印换行
    }
    JsonObject obj = p_recv_msg_doc->as<JsonObject>(); // 将文档转换为JSON对象
    String cmd = obj["cmd"]; // 获取命令
    JsonObject params = obj["params"]; // 获取参数
    send_msg_doc.clear(); // 清空发送消息文档

    if (cmd.equalsIgnoreCase("query")) // 如果命令是查询
    {
        String type = params["type"]; // 获取类型
        if (type.equals("discovery")) // 如果类型是发现
        {
            String chip_id = String(ESP.getChipId(), HEX); // 获取芯片ID
            chip_id.toUpperCase(); // 转换为大写
            send_msg_doc["cmd"] = "query_discovery"; // 设置发送命令为查询发现
            send_msg_doc["params"]["ip"] = WiFi.localIP().toString(); // 获取本地IP
            send_msg_doc["params"]["mac"] = chip_id; // 设置MAC地址

            if (ConfigData.containsKey("mqtt")) // 如果配置中包含mqtt
            {
                send_msg_doc["params"]["mqtt"] = ConfigData["mqtt"]; // 设置mqtt参数
            }

            if (ConfigData.containsKey("pin")) // 如果配置中包含pin
            {
                send_msg_doc["params"]["pin"]= ConfigData["pin"]; // 设置pin参数
            }

            if (ConfigData.containsKey("version")) // 如果配置中包含版本
            {
                send_msg_doc["params"]["version"] = ConfigData["version"]; // 设置版本参数
            }

            String ip = obj["params"]["ip"]; // 获取IP
            remote_ip.fromString(ip); // 将字符串转换为IP地址
            sendUDP(&send_msg_doc, remote_ip); // 发送UDP消息
        }
        else if (type.equals("info")) // 如果类型是信息
        {
            String free_mem = String(ESP.getFreeHeap() / 1024) + "KB"; // 获取可用内存
            String chip_id = String(ESP.getChipId(), HEX); // 获取芯片ID
            chip_id.toUpperCase(); // 转换为大写
            String cpu_freq = String(ESP.getCpuFreqMHz()) + "MHz"; // 获取CPU频率
            String flash_speed = String(ESP.getFlashChipSpeed() / 1000000); // 获取闪存速度
            String flash_size = String(ESP.getFlashChipSize() / 1024) + "KB"; // 获取闪存大小
            String sketch_size = String(ESP.getSketchSize() / 1024) + "KB"; // 获取草图大小
            String reset_reason = ESP.getResetReason(); // 获取重置原因
            String sketch_space = String(ESP.getFreeSketchSpace() / 1024) + "KB"; // 获取草图空间
            FSInfo fsinfo; // 文件系统信息结构体
            LittleFS.info(fsinfo); // 获取文件系统信息
            String fs_total_bytes = String(fsinfo.totalBytes / 1024) + "KB"; // 获取文件系统总字节
            String fs_used_bytes = String(fsinfo.usedBytes / 1024) + "KB"; // 获取文件系统已用字节
            send_msg_doc["cmd"] = "query_info"; // 设置发送命令为查询信息
            send_msg_doc["params"]["free_mem"] = free_mem; // 设置可用内存参数
            send_msg_doc["params"]["chip_id"] = chip_id; // 设置芯片ID参数
            send_msg_doc["params"]["cpu_freq"] = cpu_freq; // 设置CPU频率参数
            send_msg_doc["params"]["flash_speed"] = flash_speed; // 设置闪存速度参数
            send_msg_doc["params"]["flash_size"] = flash_size; // 设置闪存大小参数
            send_msg_doc["params"]["reset_reason"] = reset_reason; // 设置重置原因参数
            send_msg_doc["params"]["sketch_space"] = sketch_space; // 设置草图空间参数
            send_msg_doc["params"]["fs_total_bytes"] = fs_total_bytes; // 设置文件系统总字节参数
            send_msg_doc["params"]["fs_used_bytes"] = fs_used_bytes; // 设置文件系统已用字节参数
            send_msg_doc["params"]["version_name"] = FIRMWARE_VERSION; // 设置固件版本名称
            send_msg_doc["params"]["version_code"] = VERSION_CODE; // 设置固件版本代码
            returnUDP(&send_msg_doc); // 返回UDP消息
        }
    }

    if (cmd.equalsIgnoreCase("send")) // 如果命令是发送
    {
        String signal = params["signal"]; // 获取信号
        String type = params["type"]; // 获取类型

        if (signal.equalsIgnoreCase("ir")) // 如果信号是红外
        {
            if (type.equalsIgnoreCase("status")) // 如果类型是状态
            {
                String file = params["file"]; // 获取文件
                JsonObject statusJson = params[type]; // 获取状态JSON对象
                t_remote_ac_status ac_status; // 创建空调状态结构体
                ac_status.ac_power = t_ac_power((int)statusJson["power"]); // 设置空调电源状态
                ac_status.ac_temp = t_ac_temperature((int)statusJson["temperature"]); // 设置空调温度
                ac_status.ac_mode = t_ac_mode((int)statusJson["mode"]); // 设置空调模式
                ac_status.ac_swing = t_ac_swing((int)statusJson["swing"]); // 设置空调摆动
                ac_status.ac_wind_speed = t_ac_wind_speed((int)statusJson["speed"]); // 设置空调风速
                ac_status.ac_display = 1; // 设置显示状态
                ac_status.ac_timer = 0; // 设置定时器
                ac_status.ac_sleep = 0; // 设置睡眠模式
                sendStatus(file, ac_status); // 发送状态
                returnACStatus(file, ac_status); // 返回空调状态
            }
            else if (type.equalsIgnoreCase("file")) // 如果类型是文件
            {
                String file = params["file"]; // 获取文件
                sendIR(file); // 发送红外文件
            }
            else if (type.equalsIgnoreCase("key")) // 如果类型是按键
            {
                String file = params["file"]; // 获取文件
            }
            else if (type.equalsIgnoreCase("data")) // 如果类型是数据
            {
            }
            else if (type.equalsIgnoreCase("local")) // 如果类型是本地
            {
                String file = params["file"]; // 获取文件
                JsonObject localobj = params[type]; // 获取本地对象
                if (!ACStatus.containsKey(file)) { // 如果空调状态中不包含该文件
                    initAC(file); // 初始化空调
                }
                t_remote_ac_status ac_status = getACState(file); // 获取空调状态
                if (localobj.containsKey("mode")) { // 如果本地对象中包含模式
                    String mode = localobj["mode"]; // 获取模式
                    if (mode.equalsIgnoreCase("off")) // 如果模式是关闭
                        ac_status.ac_power = AC_POWER_OFF; // 设置电源为关闭
                    else
                        ac_status.ac_power = AC_POWER_ON; // 否则设置电源为开启

                    if (mode.equalsIgnoreCase("cool")) // 如果模式是制冷
                        ac_status.ac_mode = AC_MODE_COOL; // 设置模式为制冷
                    else if (mode.equalsIgnoreCase("heat")) // 如果模式是加热
                        ac_status.ac_mode = AC_MODE_HEAT; // 设置模式为加热
                    else if (mode.equalsIgnoreCase("auto")) // 如果模式是自动
                        ac_status.ac_mode = AC_MODE_AUTO; // 设置模式为自动
                    else if (mode.equalsIgnoreCase("fan") || mode.equalsIgnoreCase("fan_only")) // 如果模式是风扇或仅风扇
                        ac_status.ac_mode = AC_MODE_FAN; // 设置模式为风扇
                    else if (mode.equalsIgnoreCase("dry")) // 如果模式是除湿
                        ac_status.ac_mode = AC_MODE_DRY; // 设置模式为除湿
                } else if (localobj.containsKey("temperature")) { // 如果本地对象中包含温度
                    String temperature = localobj["temperature"]; // 获取温度
                    ac_status.ac_temp = (t_ac_temperature)(temperature.toInt() - 16); // 设置空调温度
                } else if (localobj.containsKey("fan")) { // 如果本地对象中包含风扇
                    String fan = localobj["fan"]; // 获取风扇状态
                    if (fan.equalsIgnoreCase("auto")) // 如果风扇状态是自动
                        ac_status.ac_wind_speed = AC_WS_AUTO; // 设置风速为自动
                    else if (fan.equalsIgnoreCase("low")) // 如果风扇状态是低速
                        ac_status.ac_wind_speed = AC_WS_LOW; // 设置风速为低速
                    else if (fan.equalsIgnoreCase("medium")) // 如果风扇状态是中速
                        ac_status.ac_wind_speed = AC_WS_MEDIUM; // 设置风速为中速
                    else if (fan.equalsIgnoreCase("high")) // 如果风扇状态是高速
                        ac_status.ac_wind_speed = AC_WS_HIGH; // 设置风速为高速
                } else if (localobj.containsKey("swing")) { // 如果本地对象中包含摆动
                    String swing = localobj["swing"]; // 获取摆动状态
                    if (swing.equalsIgnoreCase("on")) // 如果摆动状态是开启
                        ac_status.ac_swing = AC_SWING_ON; // 设置摆动为开启
                    else if (swing.equalsIgnoreCase("off")) // 如果摆动状态是关闭
                        ac_status.ac_swing = AC_SWING_OFF; // 设置摆动为关闭
                }
                sendStatus(file, ac_status); // 发送状态
                returnACStatus(file, ac_status); // 返回空调状态
            }
        }
#ifdef USE_RF // 如果使用RF
        else if (signal.equalsIgnoreCase("rf315")) // 如果信号是RF315
        {
            RFTYPE rf_type; // 定义RF类型

            rf_type = RF315; // 设置RF类型为RF315
            if (type.equalsIgnoreCase("data")) // 如果类型是数据
            {
                unsigned long code = params["code"]; // 获取代码
                unsigned int length = params["length"]; // 获取长度
                sendRFData(code, length, rf_type); // 发送RF数据
            }
            else if (type.equalsIgnoreCase("file")) // 如果类型是文件
            {
                String file = params["file"]; // 获取文件
                sendRFFile(file); // 发送RF文件
            }
        }
        else if (signal.equalsIgnoreCase("rf433")) // 如果信号是RF433
        {
            RFTYPE rf_type; // 定义RF类型
            if (type.equalsIgnoreCase("data")) // 如果类型是数据
            {
                rf_type = RF433; // 设置RF类型为RF433
                unsigned long code = params["code"]; // 获取代码
                unsigned int length = params["length"]; // 获取长度
                sendRFData(code, length, rf_type); // 发送RF数据
            }
            else if (type.equalsIgnoreCase("file")) // 如果类型是文件
            {
                String file = params["file"]; // 获取文件
                sendRFFile(file); // 发送RF文件
            }
        }
#endif
    }

    if (cmd.equalsIgnoreCase("set")) // 如果命令是设置
    {
        String type = params["type"]; // 获取类型
        if (type.equals("update")) { // 如果类型是更新
            String url = params["url"]; // 获取URL
            otaUpdate(url); // 执行OTA更新
        }

        else if (type.equals("record")) { // 如果类型是记录
            String ip = params["ip"]; // 获取IP
            remote_ip.fromString(ip); // 将字符串转换为IP地址
            DEBUGLN("start record"); // 打印开始记录
            enableIR(); // 启用红外
            enableRF(); // 启用RF
        }

        else if (type.equals("disable_record")) { // 如果类型是禁用记录
            DEBUGLN("disable record"); // 打印禁用记录
            disableRF(); // 禁用RF
            disableIR(); // 禁用红外
        }

        else if (type.equals("save_signal")) { // 如果类型是保存信号
            String file_name = params["file"]; // 获取文件名
            String signal = params["signal"]; // 获取信号
            if (signal.equals("IR")) // 如果信号是红外
                saveIR(file_name); // 保存红外信号
            else
                saveRF(file_name); // 保存RF信号
        }

        else if (type.equals("reset")) // 如果类型是重置
            settingsClear(); // 清除设置

        else if (type.equals("config")) { // 如果类型是配置
            if (params.containsKey("mqtt")) { // 如果参数中包含mqtt
                ConfigData["mqtt"]["host"] = params["mqtt"]["host"]; // 设置mqtt主机
                ConfigData["mqtt"]["port"] = params["mqtt"]["port"]; // 设置mqtt端口
                ConfigData["mqtt"]["user"] = params["mqtt"]["user"]; // 设置mqtt用户
                ConfigData["mqtt"]["password"] = params["mqtt"]["password"]; // 设置mqtt密码
            }
            if (params.containsKey("pin")) { // 如果参数中包含pin
                ConfigData["pin"]["ir_send"] = params["pin"]["ir_send"]; // 设置红外发送引脚
                ConfigData["pin"]["ir_receive"] = params["pin"]["ir_receive"]; // 设置红外接收引脚
            }
            send_msg_doc["cmd"] = "return"; // 设置发送命令为返回
            send_msg_doc["params"]["message"] = "set success"; // 设置返回消息为设置成功
            returnUDP(&send_msg_doc); // 返回UDP消息
            settingsSave(); // 保存设置
            mqttDisconnect(); // 断开MQTT连接
            mqttReconnect(); // 重新连接MQTT
            loadIRPin(ConfigData["pin"]["ir_send"], ConfigData["pin"]["ir_receive"]); // 加载红外引脚
        }

        else if (type.equals("device")) { // 如果类型是设备
            DEBUGLN("Register Device"); // 打印注册设备
            String file = params["file"]; // 获取文件
            int device_type = (int)params["device_type"]; // 获取设备类型
            bool exist = params["exist"]; // 获取设备是否存在
            if (device_type == 1) // 如果设备类型是1
                registAC(file, exist); // 注册空调
        }
    }
    return true; // 返回成功
}