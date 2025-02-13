#include "IRbabyIR.h" // 包含 IRbabyIR 头文件
#include "ESP8266HTTPClient.h" // 包含 ESP8266 HTTP 客户端头文件
#include "IRbabySerial.h" // 包含 IRbabySerial 头文件
#include "IRbabyUDP.h" // 包含 IRbabyUDP 头文件
#include "IRbabyUserSettings.h" // 包含 IRbabyUserSettings 头文件
#include <LittleFS.h> // 包含 LittleFS 文件系统头文件
#include "IRbabyGlobal.h" // 包含 IRbabyGlobal 头文件
#include "defines.h" // 包含定义文件

#define DOWNLOAD_PREFIX "http://irext-debug.oss-cn-hangzhou.aliyuncs.com/irda_" // 下载前缀
#define DOWNLOAD_SUFFIX ".bin" // 下载后缀
#define SAVE_PATH "/bin/" // 保存路径

decode_results results; // 存储解码结果的变量
const uint8_t kTimeout = 50; // 超时时间常量
const uint16_t kCaptureBufferSize = 1024; // 捕获缓冲区大小常量
static IRsend * ir_send = nullptr; // IR 发送对象指针
static IRrecv * ir_recv = nullptr; // IR 接收对象指针
bool saveSignal(); // 声明保存信号的函数

void downLoadFile(String file) // 下载文件的函数
{
  HTTPClient http_client; // 创建 HTTP 客户端对象
  String download_url = DOWNLOAD_PREFIX + file + DOWNLOAD_SUFFIX; // 构建下载 URL
  String save_path = BIN_SAVE_PATH + file; // 构建保存路径
  File cache = LittleFS.open(save_path, "w"); // 打开文件以写入
  if (cache) { // 如果文件成功打开
    http_client.begin(wifi_client, download_url); // 开始 HTTP 请求
    if (http_client.GET() == HTTP_CODE_OK) { // 如果请求成功
        WiFiClient &wificlient = http_client.getStream(); // 获取 WiFi 客户端流
        uint8_t buff[512]; // 创建缓冲区
        int n = wificlient.readBytes(buff, http_client.getSize()); // 读取数据
        cache.write(buff, n); // 写入缓存
        cache.flush(); // 刷新缓存
    }
  } else {
      clearBinFiles(); // 清除二进制文件
  }
  cache.close(); // 关闭文件
  http_client.end(); // 结束 HTTP 客户端
}

bool sendIR(String file_name) // 发送 IR 信号的函数
{
    String save_path = SAVE_PATH + file_name; // 构建保存路径
    if (LittleFS.exists(save_path)) // 如果文件存在
    {
        File cache = LittleFS.open(save_path, "r"); // 打开文件以读取
        if (!cache) // 如果文件打开失败
        {
            ERRORF("Failed to open %s", save_path.c_str()); // 输出错误信息
            return false; // 返回失败
        }
        Serial.println(); // 打印换行
        uint16_t *data_buffer = (uint16_t *)malloc(sizeof(uint16_t) * 512); // 分配数据缓冲区
        uint16_t length = cache.size() / 2; // 计算数据长度
        memset(data_buffer, 0x0, 512); // 清空数据缓冲区
        INFOF("file size = %d\n", cache.size()); // 输出文件大小
        INFOLN(); // 输出换行
        cache.readBytes((char *)data_buffer, cache.size()); // 读取文件数据
        ir_recv->disableIRIn(); // 禁用 IR 接收
        ir_send->sendRaw(data_buffer, length, 38); // 发送原始 IR 数据
        ir_recv->enableIRIn(); // 启用 IR 接收
        free(data_buffer); // 释放数据缓冲区
        cache.close(); // 关闭文件
        return true; // 返回成功
    }
    return false; // 返回失败
}

void sendStatus(String file, t_remote_ac_status status) // 发送状态的函数
{
    String save_path = SAVE_PATH + file; // 构建保存路径
    if (!LittleFS.exists(save_path)) // 如果文件不存在
        downLoadFile(file); // 下载文件

    if (LittleFS.exists(save_path)) // 如果文件存在
    {
        File cache = LittleFS.open(save_path, "r"); // 打开文件以读取
        if (cache) // 如果文件成功打开
        {
            UINT16 content_size = cache.size(); // 获取内容大小
            DEBUGF("content size = %d\n", content_size); // 输出内容大小

            if (content_size != 0) // 如果内容不为空
            {
                UINT8 *content = (UINT8 *)malloc(content_size * sizeof(UINT8)); // 分配内容缓冲区
                cache.seek(0L, fs::SeekSet); // 设置文件指针
                cache.readBytes((char *)content, content_size); // 读取文件内容
                ir_binary_open(REMOTE_CATEGORY_AC, 1, content, content_size); // 打开二进制数据
                UINT16 *user_data = (UINT16 *)malloc(1024 * sizeof(UINT16)); // 分配用户数据缓冲区
                UINT16 data_length = ir_decode(0, user_data, &status, FALSE); // 解码数据

                DEBUGF("data_length = %d\n", data_length); // 输出数据长度
                ir_recv->disableIRIn(); // 禁用 IR 接收
                ir_send->sendRaw(user_data, data_length, 38); // 发送解码后的数据
                ir_recv->enableIRIn(); // 启用 IR 接收
                ir_close(); // 关闭 IR
                free(user_data); // 释放用户数据缓冲区
                free(content); // 释放内容缓冲区
                saveACStatus(file, status); // 保存 AC 状态
            }
            else
                ERRORF("Open %s is empty\n", save_path.c_str()); // 输出错误信息
        }
        cache.close(); // 关闭文件
    }
}

void recvIR() // 接收 IR 信号的函数
{
    if (ir_recv->decode(&results)) // 解码 IR 信号
    {
        DEBUGF("raw length = %d\n", results.rawlen - 1); // 输出原始长度
        String raw_data; // 创建原始数据字符串
        for (int i = 1; i < results.rawlen; i++) // 遍历原始数据
            raw_data += String(*(results.rawbuf + i) * kRawTick) + " "; // 处理原始数据
        ir_recv->resume(); // 恢复 IR 接收
        send_msg_doc.clear(); // 清空发送消息文档
        send_msg_doc["cmd"] = "record_rt"; // 设置命令
        send_msg_doc["params"]["signal"] = "IR"; // 设置信号参数
        send_msg_doc["params"]["length"] = results.rawlen; // 设置长度参数
        send_msg_doc["params"]["value"] = raw_data.c_str(); // 设置值参数
        DEBUGLN(raw_data.c_str()); // 输出原始数据
        sendUDP(&send_msg_doc, remote_ip); // 发送 UDP 消息
        saveSignal(); // 保存信号
    }
}

bool saveIR(String file_name) // 保存 IR 信号的函数
{
    String save_path = SAVE_PATH; // 构建保存路径
    save_path += file_name; // 添加文件名
    return LittleFS.rename("/bin/test", save_path); // 重命名文件
}

bool saveSignal() // 保存信号的函数
{
    String save_path = SAVE_PATH; // 构建保存路径
    save_path += "test"; // 添加文件名
    DEBUGF("save raw data as %s\n", save_path.c_str()); // 输出保存路径
    File cache = LittleFS.open(save_path, "w"); // 打开文件以写入
    if (!cache) // 如果文件打开失败
    {
        ERRORLN("Failed to create file"); // 输出错误信息
        return false; // 返回失败
    }
    for (size_t i = 0; i < results.rawlen; i++) // 遍历原始数据
        *(results.rawbuf + i) = *(results.rawbuf + i) * kRawTick; // 处理原始数据
    cache.write((char *)(results.rawbuf + 1), (results.rawlen - 1) * 2); // 写入文件
    cache.close(); // 关闭文件
    return true; // 返回成功
}

void initAC(String file) // 初始化空调的函数
{
    ACStatus[file]["power"] = 0; // 设置电源状态
    ACStatus[file]["temperature"] = 8; // 设置温度
    ACStatus[file]["mode"] = 2; // 设置模式
    ACStatus[file]["swing"] = 0; // 设置摆动状态
    ACStatus[file]["speed"] = 0; // 设置风速
}

bool sendKey(String file_name, int key) // 发送按键的函数
{
    String save_path = SAVE_PATH; // 构建保存路径
    save_path += file_name; // 添加文件名
    if (LittleFS.exists(save_path)) // 如果文件存在
    {
        File cache = LittleFS.open(save_path, "r"); // 打开文件以读取
        if (cache) // 如果文件成功打开
        {
            UINT16 content_size = cache.size(); // 获取内容大小
            DEBUGF("content size = %d\n", content_size); // 输出内容大小

            if (content_size != 0) // 如果内容不为空
            {
                UINT8 *content = (UINT8 *)malloc(content_size * sizeof(UINT8)); // 分配内容缓冲区
                cache.seek(0L, fs::SeekSet); // 设置文件指针
                cache.readBytes((char *)content, content_size); // 读取文件内容
                ir_binary_open(2, 1, content, content_size); // 打开二进制数据
                UINT16 *user_data = (UINT16 *)malloc(1024 * sizeof(UINT16)); // 分配用户数据缓冲区
                UINT16 data_length = ir_decode(0, user_data, NULL, FALSE); // 解码数据

                DEBUGF("data_length = %d\n", data_length); // 输出数据长度
                if (LOG_DEBUG) // 如果调试日志开启
                {
                    for (int i = 0; i < data_length; i++) // 遍历用户数据
                        Serial.printf("%d ", *(user_data + i)); // 输出用户数据
                    Serial.println(); // 打印换行
                }
                ir_recv->disableIRIn(); // 禁用 IR 接收
                ir_send->sendRaw(user_data, data_length, 38); // 发送解码后的数据
                ir_recv->enableIRIn(); // 启用 IR 接收
                ir_close(); // 关闭 IR
                free(user_data); // 释放用户数据缓冲区
                free(content); // 释放内容缓冲区
            }
            else
                ERRORF("Open %s is empty\n", save_path.c_str()); // 输出错误信息
        }
        cache.close(); // 关闭文件
    }
    return true; // 返回成功
}

void loadIRPin(uint8_t send_pin, uint8_t recv_pin) // 加载 IR 引脚的函数
{
    if (ir_send != nullptr) { // 如果 IR 发送对象存在
        delete ir_send; // 删除 IR 发送对象
    }
    if (ir_recv != nullptr) { // 如果 IR 接收对象存在
        delete ir_recv; // 删除 IR 接收对象
    }
    ir_send = new IRsend(send_pin); // 创建新的 IR 发送对象
    DEBUGF("Load IR send pin at %d\n", send_pin); // 输出加载的引脚
    ir_send->begin(); // 初始化 IR 发送
    ir_recv = new IRrecv(recv_pin, kCaptureBufferSize, kTimeout, true); // 创建新的 IR 接收对象
    disableIR(); // 禁用 IR
}

void disableIR() // 禁用 IR 的函数
{
    ir_recv->disableIRIn(); // 禁用 IR 接收
}

void enableIR() // 启用 IR 的函数
{
    ir_recv->enableIRIn(); // 启用 IR 接收
}