#pragma once
#include <ESP8266WiFi.h>
#include <work/Work.hpp>
#include <work/WorkStatus.hpp>
#include <log/Logger.hpp>
using namespace std;
class WiFiStatus {
public:
    string ssid;
    int length;
    bool connected;
};
class WiFiStatusListenerWork :public Work , public Logger {
private:
public:
    inline string getWorkName() {
        return "WiFiStatusListenerWork";
    }
    inline void initWork(WorkStatus status) {

    }
    template <typename T>
    inline T onLoop() {
        WiFiStatus status = WiFiStatus();
        status.connected = WiFi.status() == WL_CONNECTED;
        status.ssid = WiFi.SSID().c_str();
        status.length = WiFi.RSSI();
        return status;
    }
};