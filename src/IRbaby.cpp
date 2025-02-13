/**
 *
 * Copyright (c) 2020 IRbaby
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// 包含必要的库
#include <Arduino.h> // Arduino核心库
#include <ESP8266HTTPClient.h> // ESP8266 HTTP客户端库
#include <Ticker.h> // Ticker库，用于定时器

// 包含自定义头文件
#include "IRbabyGlobal.h" // 全局设置
#include "IRbabyIR.h" // 红外相关功能
#include "IRbabyMQTT.h" // MQTT相关功能
#include "IRbabyMsgHandler.h" // 消息处理
#include "IRbabyOTA.h" // OTA更新
#include "IRbabyRF.h" // RF相关功能
#include "IRbabyUDP.h" // UDP相关功能
#include "IRbabyUserSettings.h" // 用户设置
#include "OneButton.h" // 单按钮库
#include "defines.h" // 定义常量和宏

#ifdef USE_SENSOR // 如果使用传感器
#include "IRbabyBinarySensor.h" // 二进制传感器相关功能
Ticker sensor_ticker;  // 二进制传感器定时器
#endif                 // USE_SENSOR

void uploadIP();    // 上传设备信息到devicehive
Ticker mqtt_check;  // MQTT检查定时器
Ticker disable_ir;  // 禁用红外接收定时器
Ticker disable_rf;  // 禁用RF接收定时器
Ticker save_data;   // 保存数据定时器

OneButton button(RESET_PIN, true); // 创建一个按钮对象，重置引脚为RESET_PIN

void setup() {
  if (LOG_DEBUG || LOG_ERROR || LOG_INFO) Serial.begin(BAUD_RATE); // 初始化串口
  pinMode(RESET_PIN, INPUT_PULLUP); // 设置重置引脚为上拉输入
  INFOLN(); // 打印信息
  INFOLN("8888888 8888888b.  888               888               "); // 打印欢迎信息
  INFOLN("  888   888   Y88b 888               888               ");
  INFOLN("  888   888    888 888               888               ");
  INFOLN("  888   888   d88P 88888b.   8888b.  88888b.  888  888 ");
  INFOLN("  888   8888888P   888  88b      88b 888  88b 888  888 ");
  INFOLN("  888   888 T88b   888  888 .d888888 888  888 888  888 ");
  INFOLN("  888   888  T88b  888 d88P 888  888 888 d88P Y88b 888 ");
  INFOLN("8888888 888   T88b 88888P    Y888888 88888P     Y88888 ");
  INFOLN("                                              Y8b d88P ");
  INFOLN("                                                 Y88P  ");
#ifdef USE_LED // 如果使用LED
  led.Off(); // 关闭LED
#endif  // USE_LED
  wifi_manager.autoConnect(); // 自动连接WiFi
#ifdef USE_LED // 如果使用LED
  led.On(); // 打开LED
#endif  // USE_LED

  settingsLoad();  // 从文件系统加载用户设置
  delay(5); // 延迟5毫秒
  udpInit();   // 初始化UDP
  mqttInit();  // 初始化MQTT

#ifdef USE_RF // 如果使用RF
  initRF();  // 初始化RF
#endif

  loadIRPin(ConfigData["pin"]["ir_send"], ConfigData["pin"]["ir_receive"]); // 加载红外引脚配置

#ifdef USE_INFO_UPLOAD // 如果使用信息上传
  uploadIP(); // 上传设备IP
#endif

  mqtt_check.attach_scheduled(MQTT_CHECK_INTERVALS, mqttCheck); // 定时检查MQTT
  disable_ir.attach_scheduled(DISABLE_SIGNAL_INTERVALS, disableIR); // 定时禁用红外信号
  disable_rf.attach_scheduled(DISABLE_SIGNAL_INTERVALS, disableRF); // 定时禁用RF信号
  save_data.attach_scheduled(SAVE_DATA_INTERVALS, settingsSave); // 定时保存数据

#ifdef USE_SENSOR // 如果使用传感器
  binary_sensor_init(); // 初始化二进制传感器
  sensor_ticker.attach_scheduled(SENSOR_UPLOAD_INTERVAL, binary_sensor_loop); // 设置传感器每隔10秒上传一次数据
#endif  // USE_SENSOR

  button.setPressMs(3000); // 设置按钮长按3秒
  button.attachLongPressStart([]() { settingsClear(); }); // 长按开始时清除设置
#ifdef USE_LED // 如果使用LED
  led.Blink(200, 200).Repeat(10); // 设置LED每隔200ms闪烁一次，重复10次
#endif  // USE_LED
}

void loop() {
  /* IR receive */
  recvIR(); // 接收红外信号
#ifdef USE_RF // 如果使用RF
  /* RF receive */
  recvRF(); // 接收RF信号
#endif
  /* UDP receive and handle */
  char *msg = udpRecive(); // 接收UDP消息
  if (msg) { // 如果接收到消息
    udp_msg_doc.clear(); // 清空消息文档
    DeserializationError error = deserializeJson(udp_msg_doc, msg); // 反序列化JSON
    if (error) ERRORLN("Failed to parse udp message"); // 解析失败则打印错误
    msgHandle(&udp_msg_doc, MsgType::udp); // 处理消息
  }

  /* mqtt loop */
  mqttLoop(); // 执行MQTT循环
  yield(); // 让出控制权
}

// 仅上传芯片ID
void uploadIP() {
  HTTPClient http; // 创建HTTP客户端
  StaticJsonDocument<128> body_json; // 创建JSON文档
  String chip_id = String(ESP.getChipId(), HEX); // 获取芯片ID并转换为十六进制字符串
  chip_id.toUpperCase(); // 转换为大写
  String head = "http://playground.devicehive.com/api/rest/device/"; // 设备API地址
  head += chip_id; // 添加芯片ID到地址
  http.begin(wifi_client, head); // 开始HTTP请求
  http.addHeader("Content-Type", "application/json"); // 添加请求头
  http.addHeader(
      "Authorization",
      "Bearer "
      "eyJhbGciOiJIUzI1NiJ9."
      "eyJwYXlsb2FkIjp7ImEiOlsyLDMsNCw1LDYsNyw4LDksMTAsMTEsMTIsMTUsMTYsMTddLCJl"
      "IjoxNzQzNDM2ODAwMDAwLCJ0IjoxLCJ1Ijo2NjM1LCJuIjpbIjY1NDIiXSwiZHQiOlsiKiJd"
      "fX0.WyyxNr2OD5pvBSxMq84NZh6TkNnFZe_PXenkrUkRSiw"); // 添加授权头
  body_json["name"] = chip_id; // 设置JSON文档的名称为芯片ID
  body_json["networkId"] = "6542"; // 设置网络ID
  String body = body_json.as<String>(); // 将JSON文档转换为字符串
  INFOF("update %s to devicehive\n", body.c_str()); // 打印更新信息
  http.PUT(body); // 发送PUT请求
  http.end(); // 结束HTTP请求
}