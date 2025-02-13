// src/IRbabyRF.cpp
#include "IRbabyRF.h" // 包含头文件 IRbabyRF.h
#include <LittleFS.h> // 包含 LittleFS 文件系统库
#include "IRbabySerial.h" // 包含 IRbabySerial 相关功能
#include "IRbabyGlobal.h" // 包含全局变量和常量定义
#include "IRbabyUDP.h" // 包含 UDP 相关功能
#include "defines.h" // 包含定义的常量和宏
#include <Ticker.h> // 包含 Ticker 库用于定时器

RCSwitch rf315; // 创建 RF315 对象
RCSwitch rf433; // 创建 RF433 对象

#define FILE_PRE "/rf/" // 定义文件路径前缀
unsigned long code_tmp; // 临时存储接收到的代码
unsigned int length_tmp; // 临时存储接收到的长度
RFTYPE rf_type_tmp; // 临时存储 RF 类型

Ticker change_rf_ticker; // 定义定时器用于切换接收器
RFTYPE rf_receiver; // 当前接收器类型
bool rf_record; // 是否记录 RF 数据的标志

static void changeReceiver(void) // 切换接收器的函数
{
    static bool flag = false; // 静态变量用于切换状态
    flag = !flag; // 切换状态
    rf_receiver = flag ? RF315 : RF433; // 根据状态选择接收器
}

void sendRFData(unsigned long code, unsigned int length, RFTYPE type) // 发送 RF 数据的函数
{
    if (type == RF315) // 如果类型是 RF315
    {
        rf315.send(code, length); // 发送数据
    }
    else if (type == RF433) // 如果类型是 RF433
    {
        rf433.send(code, length); // 发送数据
    }
}

bool sendRFFile(String file_name) // 发送 RF 文件的函数
{
    File cache; // 文件对象
    unsigned long code; // 存储代码
    unsigned int length; // 存储长度
    RFTYPE rf_type; // 存储 RF 类型
    String file_path = FILE_PRE + file_name; // 构建文件路径
    if (file_name.equals("test")) // 如果文件名是 "test"
    {
        code = code_tmp; // 使用临时存储的代码
        length = length_tmp; // 使用临时存储的长度
        rf_type = rf_type_tmp; // 使用临时存储的 RF 类型
    }
    else if (LittleFS.exists(file_path)) // 如果文件存在
    {
        cache = LittleFS.open(file_path, "r"); // 打开文件
        if (!cache) // 如果打开失败
        {
            return false; // 返回失败
        }
        cache.readBytes((char *)&rf_type, sizeof(rf_type_tmp)); // 读取 RF 类型
        cache.readBytes((char *)&code, sizeof(code)); // 读取代码
        cache.readBytes((char *)&length, sizeof(length)); // 读取长度
        DEBUGF("type = %d\n", rf_type); // 调试输出 RF 类型
        DEBUGF("code = %ld\n", code); // 调试输出代码
        DEBUGF("length = %d\n", length); // 调试输出长度
        cache.close(); // 关闭文件
    }
    else // 如果文件不存在
    {
        ERRORF("%s file not exists\n", file_name.c_str()); // 输出错误信息
        return false; // 返回失败
    }
    switch (rf_type) // 根据 RF 类型发送数据
    {
    case RF315:
        rf315.send(code, length); // 发送 RF315 数据
        break;

    case RF433:
        rf433.send(code, length); // 发送 RF433 数据
        break;
    default:
        break; // 默认情况不做处理
    }
    return true; // 返回成功
}

void recvRF(void) // 接收 RF 数据的函数
{
    if (rf_record) // 如果正在记录
    {
        if (rf_receiver == RF433) // 如果当前接收器是 RF433
        {
            rf433.enableReceive(R_433); // 启用 RF433 接收
            rf315.disableReceive(); // 禁用 RF315 接收
        }
        else // 否则
        {
            rf315.enableReceive(R_315); // 启用 RF315 接收
            rf433.disableReceive(); // 禁用 RF433 接收
        }
        delay(100); // 延迟 100 毫秒
        if (rf_receiver == RF433 && rf433.available()) // 如果 RF433 可用
        {
            code_tmp = rf433.getReceivedValue(); // 获取接收到的值
            length_tmp = rf433.getReceivedBitlength(); // 获取接收到的位长度
            rf_type_tmp = RF433; // 设置 RF 类型为 RF433
            rf433.resetAvailable(); // 重置可用状态
            send_msg_doc.clear(); // 清空发送消息文档
            send_msg_doc["cmd"] = "record_rt"; // 设置命令
            send_msg_doc["params"]["signal"] = "RF433"; // 设置信号类型
            send_msg_doc["params"]["value"] = String(code_tmp); // 设置值
            sendUDP(&send_msg_doc, remote_ip); // 发送 UDP 消息
            serializeJsonPretty(send_msg_doc, Serial); // 序列化并输出到串口
        }

        else if (rf_receiver == RF315 && rf315.available()) // 如果 RF315 可用
        {
            code_tmp = rf315.getReceivedValue(); // 获取接收到的值
            length_tmp = rf315.getReceivedBitlength(); // 获取接收到的位长度
            rf_type_tmp = RF315; // 设置 RF 类型为 RF315
            rf315.resetAvailable(); // 重置可用状态
            send_msg_doc.clear(); // 清空发送消息文档
            send_msg_doc["cmd"] = "record_rt"; // 设置命令
            send_msg_doc["params"]["signal"] = "RF315"; // 设置信号类型
            send_msg_doc["params"]["value"] = String(code_tmp); // 设置值
            sendUDP(&send_msg_doc, remote_ip); // 发送 UDP 消息
            serializeJsonPretty(send_msg_doc, Serial); // 序列化并输出到串口
        }
    }
}

bool saveRF(String file_name) // 保存 RF 数据的函数
{
    String save_path = FILE_PRE + file_name; // 构建保存路径
    File cache = LittleFS.open(save_path, "w"); // 打开文件以写入
    if (!cache) // 如果打开失败
    {
        return false; // 返回失败
    }
    cache.write((char *)&rf_type_tmp, sizeof(rf_type_tmp)); // 写入 RF 类型
    cache.write((char *)&code_tmp, sizeof(code_tmp)); // 写入代码
    cache.write((char *)&length_tmp, sizeof(length_tmp)); // 写入长度
    cache.close(); // 关闭文件
    return true; // 返回成功
}

void disableRF(void) // 禁用 RF 的函数
{
    rf_record = false; // 设置记录标志为 false
    rf315.disableReceive(); // 禁用 RF315 接收
    rf433.disableReceive(); // 禁用 RF433 接收
}

void enableRF(void) // 启用 RF 的函数
{
    rf_record = true; // 设置记录标志为 true
}

void initRF(void) // 初始化 RF 的函数
{
    rf_record = false; // 设置记录标志为 false
    rf315.enableTransmit(T_315); // 启用 RF315 发送
    rf433.enableTransmit(T_433); // 启用 RF433 发送
    change_rf_ticker.attach_ms_scheduled(100, changeReceiver); // 设置定时器每 100 毫秒调用一次 changeReceiver
}