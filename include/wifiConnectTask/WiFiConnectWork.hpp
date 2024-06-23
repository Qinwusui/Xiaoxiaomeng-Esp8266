#pragma once
#include <work/Work.hpp>
#include <log/Logger.hpp>
#include <ESP8266WiFi.h>
using namespace std;
//定义一个WiFi连接任务，实现Work接口
class WiFiConnectWork :public Work , public Logger {
private:
    //定义WiFi SSID , Password
    string ssid , pwd;
public:

    WiFiConnectWork(string ssid , string pwd = "") {
        this->ssid = ssid;
        this->pwd = pwd;
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        WiFi.begin(String(ssid.c_str()) , String(pwd.c_str()));

    };
    inline string getWorkName() {
        return "WiFiConnectWork";
    }
    inline void initWork(WorkStatus status) {
        status.init(getWorkName());
        Println(getWorkName() , ssid);
        if (WiFi.status() != WL_CONNECTED) {
            status.running(getWorkName());
            return;
        }
        Println(getWorkName() , string("WiFi已连接")
            .append(" IP:")
            .append(WiFi.localIP().toString().c_str())
        );
        status.finished(getWorkName());
    }
    inline void onLoop() {
            
    }
};
