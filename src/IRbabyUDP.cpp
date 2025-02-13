#include <WiFiUdp.h> // 包含 WiFi UDP 库
#include <ESP8266WiFi.h> // 包含 ESP8266 WiFi 库
#include "IRbabyUDP.h" // 包含 IRbabyUDP 头文件
#include "IRbabySerial.h" // 包含 IRbabySerial 头文件

WiFiUDP udp; // 创建 WiFiUDP 对象
IPAddress remote_ip; // 定义远程 IP 地址
char incomingPacket[UDP_PACKET_SIZE]; // 缓冲区，用于接收传入的数据包

void udpInit() // 初始化 UDP
{
    udp.begin(UDP_PORT); // 在指定端口开始监听 UDP
    DEBUGF("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), UDP_PORT); // 输出当前 IP 和端口
}

char *udpRecive() // 接收 UDP 数据
{
    int packetSize = udp.parsePacket(); // 解析数据包
    if (packetSize) // 如果有数据包
    {
        /* 接收传入的 UDP 数据包 */
        DEBUGF("Received %d bytes from %s, port %d\n", packetSize, udp.remoteIP().toString().c_str(), udp.remotePort()); // 输出接收到的数据包信息
        int len = udp.read(incomingPacket, 255); // 读取数据包内容
        if (len > 0) // 如果读取成功
        {
            incomingPacket[len] = 0; // 在字符串末尾添加结束符
        }
        return incomingPacket; // 返回接收到的数据包
    }
    return nullptr; // 如果没有数据包，返回 nullptr
}

uint32_t sendUDP(StaticJsonDocument<1024>* doc, IPAddress ip) // 发送 UDP 数据
{
    DEBUGF("return message %s\n", ip.toString().c_str()); // 输出要发送的消息的 IP
    size_t len = serializeJson(*doc, buffer); // 序列化 JSON 文档
    udp.beginPacket(ip, UDP_PORT); // 开始发送数据包到指定 IP 和端口
    udp.write(buffer, len); // 发送序列化后的数据
    return udp.endPacket(); // 结束数据包并返回结果
}

uint32_t returnUDP(StaticJsonDocument<1024>* doc) // 返回 UDP 数据
{
    DEBUGF("return message to %s\n", udp.remoteIP().toString().c_str()); // 输出返回消息的目标 IP
    size_t len = serializeJson(*doc, buffer); // 序列化 JSON 文档
    return udp.sendPacket(buffer, len, remote_ip); // 发送数据包
}