#include "IRbabyOTA.h" // 包含头文件 IRbabyOTA.h
#include <ESP8266HTTPClient.h> // 包含 ESP8266 HTTP 客户端库
#include <ESP8266httpUpdate.h> // 包含 ESP8266 HTTP 更新库
#include <ESP8266WiFi.h> // 包含 ESP8266 WiFi 库

void update_started() // 更新开始的回调函数
{
    DEBUGLN("CALLBACK:  HTTP update process started"); // 打印更新开始的调试信息
}

void update_finished() // 更新完成的回调函数
{
    DEBUGLN("CALLBACK:  HTTP update process finished"); // 打印更新完成的调试信息
}

void update_progress(int cur, int total) // 更新进度的回调函数
{
    DEBUGF("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total); // 打印当前进度
}

void update_error(int err) // 更新错误的回调函数
{
    DEBUGF("CALLBACK:  HTTP update fatal error code %d\n", err); // 打印错误代码
}

void versionCheck() // 版本检查函数（当前为空）
{
}

void otaUpdate(String url) // OTA 更新函数，接受一个 URL 参数
{
    WiFiClient client; // 创建 WiFi 客户端
    // 添加可选的回调通知
    ESPhttpUpdate.onStart(update_started); // 设置更新开始的回调
    ESPhttpUpdate.onEnd(update_finished); // 设置更新结束的回调
    ESPhttpUpdate.onProgress(update_progress); // 设置更新进度的回调
    ESPhttpUpdate.onError(update_error); // 设置更新错误的回调

    t_httpUpdate_return ret = ESPhttpUpdate.update(client, url); // 执行更新并获取返回值

    switch (ret) // 根据返回值进行处理
    {
    case HTTP_UPDATE_FAILED: // 更新失败的情况
        DEBUGF("HTTP_UPDATE_FAILD Error (%d): %s\n", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str()); // 打印失败信息
        break;

    case HTTP_UPDATE_NO_UPDATES: // 没有更新的情况
        DEBUGLN("HTTP_UPDATE_NO_UPDATES"); // 打印没有更新的信息
        break;

    case HTTP_UPDATE_OK: // 更新成功的情况
        DEBUGLN("HTTP_UPDATE_OK"); // 打印更新成功的信息
        break;
    }
}