#pragma once
using namespace std;
#include <work/Work.hpp>
#include <log/Logger.hpp>
#include <WiFiUdp.h>
#include <NTPClient.h>

//定义一个时间校准任务
class TimeClientWork :public Work , public Logger {
private:
    WiFiUDP wifiUdp;
    NTPClient* client = NULL;
public:
    TimeClientWork() {
        this->client = new NTPClient(this->wifiUdp , "ntp.aliyun.com");
    }
    inline string getWorkName() {
        return "TimeClientWork";
    }
    inline void initWork(
        WorkStatus status
    ) {
        status.init(getWorkName());
        this->client->begin();
        status.finished(getWorkName());
    }
    inline void onLoop() {
        this->client->update();
    }
};