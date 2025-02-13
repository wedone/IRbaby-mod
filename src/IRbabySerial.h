/**********************
 * 串口打印设置
**********************/

#ifndef IRBABY_SERIAL_H // 防止重复包含头文件
#define IRBABY_SERIAL_H

#include <Arduino.h> // 包含 Arduino 库
#include "defines.h" // 包含自定义定义文件

// 定义调试输出宏
#define DEBUGLN(...) \
    {if (LOG_DEBUG) { Serial.printf("DEBUG:\t"); Serial.println(__VA_ARGS__);}} // 调试信息输出并换行

#define DEBUGF(...) \
    {if (LOG_DEBUG) { Serial.printf("DEBUG:\t"); Serial.printf(__VA_ARGS__);}} // 调试信息输出，不换行

#define DEBUG(...)  \
    {if (LOG_DEBUG) { Serial.printf("DEBUG:\t"); Serial.print(__VA_ARGS__);}} // 调试信息输出，使用 print

// 定义信息输出宏
#define INFOLN(...) \
    {if (LOG_INFO) { Serial.printf("INFO:\t"); Serial.println(__VA_ARGS__);}} // 信息输出并换行

#define INFOF(...) \
    {if (LOG_INFO) { Serial.printf("INFO:\t"); Serial.printf(__VA_ARGS__);}} // 信息输出，不换行

#define INFO(...)  \
    {if (LOG_INFO) { Serial.printf("INFO:\t"); Serial.print(__VA_ARGS__);}} // 信息输出，使用 print

// 定义错误输出宏
#define ERRORLN(...) \
    {if (LOG_ERROR) { Serial.printf("ERROR:\t"); Serial.println(__VA_ARGS__);}} // 错误信息输出并换行

#define ERRORF(...) \
    {if (LOG_ERROR) { Serial.printf("ERROR:\t"); Serial.printf(__VA_ARGS__);}} // 错误信息输出，不换行

#define ERROR(...)  \
    {if (LOG_ERROR) { Serial.printf("ERROR:\t"); Serial.print(__VA_ARGS__);}} // 错误信息输出，使用 print

#endif // IREASY_SERIAL_H // 结束条件编译