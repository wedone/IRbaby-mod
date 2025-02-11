// src/IRbabyUserSettings.cpp

#include "IRbabyUserSettings.h" // 包含用户设置头文件
#include "IRbabySerial.h"       // 包含串口通信头文件
#include "IRbabyMQTT.h"         // 包含MQTT通信头文件
#include "WiFiManager.h"        // 包含WiFi管理头文件
#include "IRbabyGlobal.h"       // 包含全局变量头文件
#include "IRbabyIR.h"           // 包含红外控制头文件
#include "defines.h"            // 包含定义头文件
#include <LittleFS.h>           // 包含LittleFS文件系统头文件

StaticJsonDocument<1024> ConfigData; // 创建配置数据的JSON文档
StaticJsonDocument<1024> ACStatus;    // 创建空调状态的JSON文档

bool settingsSave() // 保存设置的函数
{
    DEBUGLN("Save Config"); // 调试输出保存配置
    File cache = LittleFS.open("/config", "w"); // 打开配置文件进行写入
    if (!cache || (serializeJson(ConfigData, cache) == 0)) { // 检查文件是否打开成功并序列化JSON
        ERRORLN("Failed to save config file"); // 输出错误信息
        cache.close(); // 关闭文件
        return false; // 返回失败
    }
    cache.close(); // 关闭文件
    cache = LittleFS.open("/acstatus", "w"); // 打开空调状态文件进行写入
    if (!cache || (serializeJson(ACStatus, cache) == 0)) // 检查文件是否打开成功并序列化JSON
    {
        ERRORLN("ERROR: Failed to save acstatus file"); // 输出错误信息
        cache.close(); // 关闭文件
        return false; // 返回失败        
    }
    cache.close(); // 关闭文件
    return true; // 返回成功
}

bool settingsLoad() // 加载设置的函数
{
    LittleFS.begin(); // 初始化LittleFS
    int ret = false; // 初始化返回值
    FSInfo64 info; // 文件系统信息
    LittleFS.info64(info); // 获取文件系统信息
    DEBUGF("fs total bytes = %llu\n", info.totalBytes); // 输出文件系统总字节数
    if (LittleFS.exists("/config")) // 检查配置文件是否存在
    {
        File cache = LittleFS.open("/config", "r"); // 打开配置文件进行读取
        if (!cache) // 检查文件是否打开成功
        {
            ERRORLN("Failed to read config file"); // 输出错误信息
            return ret; // 返回失败
        }
        if (cache.size() > 0) // 检查文件大小
        {
            DeserializationError error = deserializeJson(ConfigData, cache); // 反序列化JSON
            if (error) // 检查反序列化是否成功
            {
                ERRORLN("Failed to load config settings"); // 输出错误信息
                return ret; // 返回失败
            }
            INFOLN("Load config data:"); // 输出加载的配置数据
            ConfigData["version"] = FIRMWARE_VERSION; // 设置版本信息
            serializeJsonPretty(ConfigData, Serial); // 美化输出JSON到串口
            Serial.println(); // 输出换行
        }
        cache.close(); // 关闭文件
    } else {
        DEBUGLN("Don't exist config"); // 输出配置文件不存在的信息
    }

    if (LittleFS.exists("/acstatus")) { // 检查空调状态文件是否存在
        File cache = LittleFS.open("/acstatus", "r"); // 打开空调状态文件进行读取
        if (!cache) { // 检查文件是否打开成功
            ERRORLN("Failed to read acstatus file"); // 输出错误信息
            return ret; // 返回失败
        }
        if (cache.size() > 0) { // 检查文件大小
            DeserializationError error = deserializeJson(ACStatus, cache); // 反序列化JSON
            if (error) { // 检查反序列化是否成功
                ERRORLN("Failed to load acstatus settings"); // 输出错误信息
                return ret; // 返回失败
            }
        }
        cache.close(); // 关闭文件
    }
    ret = true; // 设置返回值为成功
    return ret; // 返回成功
}

void settingsClear() // 清除设置的函数
{
    ESP.eraseConfig(); // 擦除ESP配置
    LittleFS.format(); // 格式化LittleFS
    ESP.reset(); // 重置ESP
}

bool saveACStatus(String file, t_remote_ac_status status) // 保存空调状态的函数
{
    bool ret = false; // 初始化返回值
    ACStatus[file]["power"] = (int)status.ac_power; // 保存空调电源状态
    ACStatus[file]["temperature"] = (int)status.ac_temp; // 保存空调温度
    ACStatus[file]["mode"] = (int)status.ac_mode; // 保存空调模式
    ACStatus[file]["swing"] = (int)status.ac_swing; // 保存空调摆动状态
    ACStatus[file]["speed"] = (int)status.ac_wind_speed; // 保存空调风速
    return ret; // 返回状态
}

t_remote_ac_status getACState(String file) // 获取空调状态的函数
{
    t_remote_ac_status status; // 初始化空调状态
    int power = (int)ACStatus[file]["power"]; // 获取电源状态
    int temperature = (int)ACStatus[file]["temperature"]; // 获取温度
    int mode = (int)ACStatus[file]["mode"]; // 获取模式
    int swing = (int)ACStatus[file]["swing"]; // 获取摆动状态
    int wind_speed = (int)ACStatus[file]["speed"]; // 获取风速
    status.ac_power = (t_ac_power)power; // 设置电源状态
    status.ac_temp = (t_ac_temperature)temperature; // 设置温度
    status.ac_mode = (t_ac_mode)mode; // 设置模式
    status.ac_swing = (t_ac_swing)swing; // 设置摆动状态
    status.ac_wind_speed = (t_ac_wind_speed)wind_speed; // 设置风速
    return status; // 返回空调状态
}

void clearBinFiles() { // 清除二进制文件的函数
    Dir root = LittleFS.openDir(BIN_SAVE_PATH); // 打开二进制保存路径目录
    while (root.next()) { // 遍历目录中的文件
        LittleFS.remove(BIN_SAVE_PATH + root.fileName()); // 删除文件
    }
}