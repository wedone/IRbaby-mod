src/defines.h
/*
 * @Author: Caffreyfans
 * @Date: 2021-07-06 20:59:02
 * @LastEditTime: 2021-07-12 23:47:26
 * @Description: 
 */ 

#ifndef _DEFINES_H // 防止重复包含
#define _DEFINES_H

typedef unsigned int uint32_t; // 定义无符号32位整数类型
typedef int int32_t;           // 定义32位整数类型
typedef unsigned short uint16_t; // 定义无符号16位整数类型
typedef short int16_t;        // 定义16位整数类型
typedef unsigned char uint8_t; // 定义无符号8位整数类型

#define FIRMWARE_VERSION "v1.0" // 固件版本名称
#define VERSION_CODE 10          // 版本代码

/* log settings */ // 日志设置
#define BAUD_RATE 115200 // 波特率
#ifndef LOG_DEBUG
#define LOG_DEBUG false // 调试日志开关
#endif
#ifndef LOG_INFO
#define LOG_INFO true // 信息日志开关
#endif
#ifndef LOG_ERROR
#define LOG_ERROR true // 错误日志开关
#endif

/* ----------------- user settings ----------------- */ // 用户设置
/* mqtt settings */ // MQTT 设置
#define MQTT_CHECK_INTERVALS 15      // 检查间隔（秒）
#define MQTT_CONNECT_WAIT_TIME 20000 // MQTT 连接等待时间（毫秒）

/* receive disable */ // 接收禁用
#define DISABLE_SIGNAL_INTERVALS 600 // 禁用信号间隔（秒）

#define SAVE_DATA_INTERVALS 300 // 保存数据间隔（秒）

// uncomment below to enable RF // 取消注释以启用 RF
// #define USE_RF

// uncomment below to enable upload chip id to remote server // 取消注释以启用上传芯片 ID 到远程服务器
#define USE_INFO_UPLOAD

// uncomment below to enable binary sensor // 取消注释以启用二进制传感器
#ifdef USE_SENSOR
#define SENSOR_UPLOAD_INTERVAL      10  // 传感器上传间隔（秒）
#endif // USE_SENSOR
/* ----------------- default pin setting --------------- */ // 默认引脚设置
/* reset pin */ // 重置引脚
#define RESET_PIN 4 // 重置引脚编号
#define LED_PIN   15 // LED 引脚编号
/* 315 RF pin */ // 315 RF 引脚
#define T_315 14 // 315 RF 发送引脚
#define R_315 5  // 315 RF 接收引脚

/* 433 RF pin */ // 433 RF 引脚
#define T_433 14 // 433 RF 发送引脚
#define R_433 5  // 433 RF 接收引脚

/* IR pin */ // 红外引脚
#define T_IR 14 // 红外发送引脚
#define R_IR 5  // 红外接收引脚

/* Sensor Pin*/ // 传感器引脚
#define SENSOR_PIN 1 // 传感器引脚编号
#endif  // _DEFINES_H // 结束防止重复包含