//导入Arduino.h基础库
#include <Arduino.h>
//导入WiFi库
#include <ESP8266WiFi.h>
//允许查询任务状态
#define _TASK_STATUS_REQUEST
//允许任务函数内联
#define _TASK_INLINE
//允许使用标准函数
#define _TASK_STD_FUNCTION

#include <functional>
#include <TaskScheduler.h>
//导入WiFi连接任务
#include <wifiConnectTask/WiFiConnectWork.hpp>
//导入时间校准任务
#include <ntp/NtpTimeClientWork.hpp>
//导入创建web Server任务
// #include <webServerTask/WebServerWork.hpp>
//导入初始化文件系统任务
#include <fileSystemWork/FileSystemInitWork.hpp>
//导入WiFi状态监听任务
#include <wifiConnectTask/WiFiStatusListenerWork.hpp>
//导入串口监听器
#include <serial/ReplaySerialWork.hpp>
//导入HTTP请求客户端
#include <httpClient/HttpClient.hpp>
//导入WebSocket客户端
#include <websocketClient/WebSocketClient.hpp>

//导入SSD1306显示库
// #include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// #include <oledTask/oledCharacters.hpp>
#include <Wire.h>
#include <U8g2lib.h>

//创建一个WebServer任务
// void createWebServer();
//创建一个WebSocket客户端启动任务
void createWsClientTask();
//创建一个WebSocket客户端轮询任务
void wsLoopTask();
//创建一个wsLoop任务
void wsLoop();
//创建一个连接WiFi任务
void connectWiFiTask();
//创建一个初始化全局变量的任务
void initAll();
//创建一个时间校准任务
void timeClientInit();
//创建一个时间更新任务
void timeUpdate();
//创建一个串口通信任务
void serialLoop();
//创建一个初始化屏幕任务
void initScreen();
//创建一个屏幕循环任务
void screenLoop();

//创建一个绘制屏幕的函数
void showText(uint8_t x , uint8_t y , const char* text);
//创建一个WiFi监听任务
void wifiStatusListener();
//命令处理
void commandHandler(String command);
