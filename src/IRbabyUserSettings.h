/*
 * @Author: Caffreyfans  // 作者: Caffreyfans
 * @Date: 2021-07-06 20:59:02  // 日期: 2021-07-06 20:59:02
 * @LastEditTime: 2021-07-12 21:24:00  // 最后编辑时间: 2021-07-12 21:24:00
 * @Description:  // 描述:
 */
#ifndef IRBABY_USER_SETTINGS_H  // 如果没有定义 IRBABY_USER_SETTINGS_H
#define IRBABY_USER_SETTINGS_H  // 定义 IRBABY_USER_SETTINGS_H
#include <ArduinoJson.h>  // 包含 ArduinoJson 库

#include "../lib/Irext/include/ir_ac_control.h"  // 包含红外空调控制库

#define BIN_SAVE_PATH "/bin/"  // 定义二进制保存路径

/* 保存配置信息 */  // 函数说明: 保存配置信息
bool settingsSave();  // 声明 settingsSave 函数

/* 清除配置信息 */  // 函数说明: 清除配置信息
void settingsClear();  // 声明 settingsClear 函数

/* 加载配置信息 */  // 函数说明: 加载配置信息
bool settingsLoad();  // 声明 settingsLoad 函数

bool saveACStatus(String, t_remote_ac_status);  // 声明 saveACStatus 函数
t_remote_ac_status getACState(String file);  // 声明 getACState 函数

void clearBinFiles();  // 声明 clearBinFiles 函数
extern StaticJsonDocument<1024> ConfigData;  // 声明外部变量 ConfigData
extern StaticJsonDocument<1024> ACStatus;  // 声明外部变量 ACStatus
#endif  // IRBABY_USER_SETTINGS_H  // 结束条件编译