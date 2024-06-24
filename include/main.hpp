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
#include <netClientTask/NtpTimeClientWork.hpp>
//导入创建web Server任务
#include <webServerTask/WebServerWork.hpp>
//导入初始化文件系统任务
#include <fileSystemWork/FileSystemInitWork.hpp>
//创建一个WebServer任务
void createWebServer();
//创建一个连接WiFi任务
void connectWiFiTask();
//创建一个初始化全局变量的任务
void initAll();
//创建一个时间校准任务
void timeClientInit();
//创建一个时间更新任务
void timeUpdate();