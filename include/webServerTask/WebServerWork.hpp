#pragma once
#include <work/Work.hpp>
#include <work/WorkStatus.hpp>
#include <log/Logger.hpp>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
using namespace std;
//定义一个启动webServer服务的任务
class WebServerWork :public Work , public Logger {
private:
    u_int port = 80;
    AsyncWebServer* server = NULL;


public:

    WebServerWork(u_int port = 80) {
        this->port = port;
        this->server = new AsyncWebServer(port);

    }
    inline string getWorkName() {
        return "WebServerWork";
    }
    inline void initWork(WorkStatus status) {

        status.init(getWorkName());
        //这是一个经典的非阻塞的异步请求
        /**
         * 该代码块表示客户端向ESP8266发起一个请求，请求地址为/wifi/scan，请求方法为GET
         * [AsyncWebServer::on()]中接收一个类型为std::function<void(AsyncWebServerRequest*)>的lambda函数
         * 在这个lambda函数中，需要实现WiFi异步扫描的业务
         * 也就是说，请求并不是立即完成的，而是会等待WiFi扫描完成后再完成
         * 通过WiFi.scanNetworksAsync()函数进行异步扫描，该函数接收一个类型为std::function<void (int)>的lambda函数，
         * 通过上述说明，我们需要把std::function<void(AsyncWebServerRequest*)>类型与std::function<void (int)>类型进行合并，产生一个新的函数，该函数
         * 能够同时接收上述两个lambda函数中的两个可传递参数AsyncWebServerRequest* 与 int
         * 为实现该功能，我们需要进行以下操作
         * ===
         * 1. 定义一个函数scanCompleteFunc，函数参数中同时包含AsyncWebServerRequest* 与 int两种类型
         * 2. 定义一个变量，类型为std::function<void(int)>，因为WiFi.scanNetworksAsync()函数只能接收该类型的参数。
         * 3. 使用std::bind()函数将定义的scanCompleteFunc函数作为待绑定函数传入bind函数，将request对象传入bind函数作为scanCompleteFunc函数的第一个参数
         * 4. 因为scanCompleteFunc函数有两个参数，但是目前只传递了一个request参数，还有一个参数是由WiFi.scanNetworksAsync函数中的lambda函数提供的，所以我们在这里需要使用占位符，即std::placeholder
         * 5. 因为占位符是占用函数的第一个未传入参数，所以值传入std::placeholders::_1
         * 6. 最后调用WiFi.scanNetworksAsync(scanFunc)执行异步扫描
         * 当WiFi扫描完成后，将会调用scanCompleteFunc函数，并传递request对象与扫描到的WiFi数量，此时我们就可以对扫描到的WiFi进行处理。
         */
        server->on("/wifi/scan" , HTTP_GET , [] (AsyncWebServerRequest* request) {
            //比较复杂的类型，可以使用auto代替
            auto   scanFunc = bind(scanCompleteFunc , request , placeholders::_1);
            WiFi.scanNetworksAsync(scanFunc);
            }
        );
        server->begin();
        status.finished(getWorkName());
    }
    //静态方法，使其能够被std::bind抹去类型
    static void scanCompleteFunc(AsyncWebServerRequest* request , int count) {
        DynamicJsonDocument json(600);
        for (int i = 0; i < count; i++) {
            const char* ssid = WiFi.SSID(i).c_str();
            json ["list"]["ssid"] = ssid;
        }
        string* jsonStr = new string();
        serializeJsonPretty(json , *jsonStr);
        request->send(200 , "application/json" , String(*jsonStr->c_str()));
        //回收对象
        delete jsonStr;
    }
    inline void onLoop() {

    }
};

