# IRbaby
## Build State
[![Build Status](https://www.travis-ci.org/Caffreyfans/IRbaby-firmware.svg?branch=master)](https://www.travis-ci.org/Caffreyfans/IRbaby-firmware)

[English doc](README.md) | [中文文档](README_zh.md)

## Feature
- [x] OTA
- [x] MQTT
- [x] IRsend
- [x] IRreceive
- [x] LED
- [x] KEY
# IRbaby
**中文版 | [English](README_en.md)**

IRbaby 使用 [IRext](https://github.com/irext/irext-core) 开源红外库(**由于某些原因该仓库已关闭,相关网站已下架,但是码库服务仍然提供**)，提供数以万计的红外设备遥控编码。IRbaby 是一个 ESP8266 万能红外方案，配合硬件支持达到类似于市面上售卖的万能红外遥控。并且只需对其进行简单设置就可以快速部署在[HomeAssisant](https://www.home-assistant.io)。

[![forthebadge](https://forthebadge.com/images/badges/built-with-love.svg)](https://forthebadge.com)

---

## 特点

* IRext 强大红外码库
* 基于 ESP8266 的芯片
* 提供 MQTT API
* 提供 UDP API
* 支持录码
* 离线解码
* HomeAssistant 自动发现
* LED 工作指示灯
---

## 架构图
![struction](/src/architecture.svg)
## 开始使用
> 1. **下载 ESP8266 固件并烧写到设备。[IRbaby-firmware](https://github.com/Caffreyfans/IRbaby-firmware/releases)**
> 2. **设备上电，移动端搜索连接到 `ESP**` 信号，并在浏览器中输入 `192.168.4.1` 对设备进行联网设置**
> 3. **下载 `Android` 客户端并运行,对设备进行 MQTT 和红外收发引脚设定。[IRbaby-android](https://github.com/Caffreyfans/IRbaby-android/releases)**
> 4. **匹配电器，完成控制, HomeAssistant 用户可在控制界面导出配置文件（现已支持 HomeAssistant 自动发现功能，设备添加之后，可直接在 HA 集成中看到）**

> **IRbaby目前仍处于开发阶，目前的交互协议可能随时改变，不保证向后兼容，升级新版本时需要注意公告说明同时升级固件和客户端。**

## 六步连接HomeAssistant
**[效果演示视频](https://www.bilibili.com/video/BV13K411j7QD)**

||||
|---|---|---|
|![发现设备](/src/discovery.jpg) |![配置信息](/src/device_setting.jpg) |![添加电器](/src/select.jpg) |
|![匹配电器](/src/parse.jpg) |![已有电器](/src/main.jpg) |![导出MQTT](/src/mqtt.jpg) |

## 材料
### 红外接收头可选(如果需要录码功能)
|||
|---|---|
|![Nodemcu](/src/nodemcu.jpg) | ![红外二级管](/src/ir_led.jpg) |
![红外接收头](/src/ir_receiver.jpg) | ![三级管](/src/transistor.jpg) |

## 关于连线

![接线](/src/connect.jpg)

`备注：红外二级管连接的时候也可以尝试不用三级管，直接连接。红外二级管长引脚接gpio，短脚接地。红外接收头的话就照着上图标示的那样与模块连接。红外接收头非必须，如果你不使用录码功能可忽略红外接收头。只要你有一个红外发射管和一块 ESP8266 和一部 Android 手机就可以尝试该项目。另外目前项目只支持空调控制，其他功能暂不支持，后续会添加。控制客户端目前也只支持 Android，跨平台客户端也在后续添加中`

## 附加下载地址
如果你有在 **github releases** 下载文件过慢的问题，请在 [https://irbaby.caffreyfans.top](https://irbaby.caffreyfans.top) 下在对应文件

## 捐赠
|支付宝|微信|
|---|---|
<img src="/src/donate-alipay.jpg" align="left" height="160" width="160">  |  <img src="/src/donate-wechat.jpg" align="left" height="160" width="160">

## 特别感谢
[Strawmanbobi](https://github.com/strawmanbobi) IRext开源库的作者，给予我技术和精神上的支持。

# ORVIBO-CT30W

欧瑞博 CT30W 智能红外遥控器主板焊有一片 OW8266-02Q、一颗白色 LED、一颗红外接收头（未被使用）、四颗红外发射头。支持通过 2.4G WiFi 和家亲 APP 连接，目前售价 10 元左右。

![board](.assets/board.png)

官方固件默认波特率：74880

## 引脚定义

| 功能名称     | GPIO 引脚 |
|-------------|-----------|
| 板载按钮     | GPIO4     |
| 板载白色 LED | GPIO15    |
| 红外接收     | GPIO5     |
| 红外发送     | GPIO14    |

![pinout](.assets/pinout.png)

## ESPHOME
ESPHOME board should be esp_wroom_02(only 2MByte spi flash)

OW8266-02Q 与 WT8266-S1 非常相似，资料可从此获得：[WT8266-S1 WiFi Module Based on ESP8266](https://core-electronics.com.au/wt8266-s1-wifi-module-based-on-esp8266.html)。

## 烧录自定义固件

前提条件：

1. CH341 等 USB 转 TTL 工具
2. 五根杜邦线、排针
3. 电烙铁

主板测试点间距 2.0mm，可以买 2.0 排针焊接上去，也可以用 2.54 排针掰一下焊接上去。

以下测试点通过杜邦线分别连接到 CH341 引脚上：

| CT30W       | CH341     |
|-------------|-----------|
| 3V3         | VCC       |
| GND         | GND       |
| RST         | 不接      |
| GPIO0       | GND       |
| RX          | TX        |
| TX          | RX        |

将 CH341 连接电脑，此时 OW8266 模块为下载模式，可以通过 Arduino 或是其他烧录工具直接下载固件到模块。下载完后拔掉 GPIO0 重新上电即为正常启动模式。

可以使用在线烧录 ESP 固件工具（需要使用 Chromium 系浏览器）：https://esp.huhn.me/


## 固件

 - ow8266 文件夹为 Arduino 发送接收红外示例，可以使用 Arduino 工具直接下载；
 - https://github.com/Mixiaoxiao/ESP8266-IR-HOMEKIT

# IRbaby-Firmware
## Build State
[![Build Status](https://www.travis-ci.org/Caffreyfans/IRbaby-firmware.svg?branch=master)](https://www.travis-ci.org/Caffreyfans/IRbaby-firmware)

[English doc](README.md) | [中文文档](README_zh.md)

## Feature
- [x] OTA
- [x] MQTT
- [x] IRsend
- [x] IRreceive
- [x] LED
- [x] KEY