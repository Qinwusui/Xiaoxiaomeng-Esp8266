#pragma once
#include <work/Work.hpp>
#include <log/Logger.hpp>
#include <ESP8266WiFi.h>
#include <vector>
#include <LittleFS.h>
class WiFiEndPoint {

public:
    string ssid;
    string pwd;
    WiFiEndPoint(string ssid , string pwd) {
        this->ssid = ssid;
        this->pwd = pwd;
    }
};

using namespace std;
//定义一个WiFi连接任务，实现Work接口
class WiFiConnectWork :public Work , public Logger {
private:
    //定义一个存储WiFi的列表
    vector<WiFiEndPoint> endPoints;
    string splitStr = "<||>";
    File f;
    string wifiListFileName = "/wifiList.txt";
public:
    //有参构造函数，指定连接的SSID与PWD，
    WiFiConnectWork(string ssid , string pwd) {
        ::WiFiConnectWork();
        WiFi.disconnect();
        WiFi.begin(String(ssid.c_str()) , String(pwd.c_str()));

    }
    WiFiConnectWork() {
        WiFi.mode(WIFI_AP_STA);
        WiFi.scanNetworks();
        //建立AP
        WiFi.softAP("wusui_esp8266" , "qinsansui233" , 2 , 0 , 20);

        if (!LittleFS.begin()) {
            Println(getWorkName() , "LittleFS 初始化失败了,不寻找待连接的WiFi列表");
            return;
        }
        f = LittleFS.open(wifiListFileName.c_str() , "w+");
        if (!f) {
            Println(getWorkName() , "获取不到WiFi列表文件,创建一个新文件");
            f.print("");
            f.flush();
        }

        f = LittleFS.open(wifiListFileName.c_str() , "r");

        String text = f.readString();
        Println(getWorkName() , "获取到的待连接WiFi列表：" + string(text.c_str()));
        int index;
        while ((index = text.indexOf('\n')) != -1) {
            String cut = text.substring(0 , index);
            String next = text.substring(index + 1 , text.length());
            text = next;
            int ssidIndex = cut.indexOf(splitStr.c_str());
            String ssid = cut.substring(0 , ssidIndex);
            String pwd = cut.substring(ssidIndex + splitStr.length() , cut.length());
            //存入WiFi接入点
            endPoints.push_back(WiFiEndPoint(ssid.c_str() , pwd.c_str()));
            Println(getWorkName() , "读取到的WiFi:" + string(ssid.c_str()) + "，密码:" + string(pwd.c_str()));
        }
        f.close();
        addWiFi("wusui_2.4G" , "Qinsansui233...");
    };
    inline bool addWiFi(string ssid , String pwd) {
        for (auto&& endPoint : endPoints) {
            if (endPoint.ssid == ssid) {
                Println(getWorkName() , "已存在相同SSID的WiFi" + ssid);
                return false;
            }
        }
        endPoints.push_back(WiFiEndPoint(ssid.c_str() , pwd.c_str()));
        if (!LittleFS.begin()) {
            Println(getWorkName() , "文件系统初始化失败,新增的WiFi无法正确保存到文件中");
            return false;
        }
        f = LittleFS.open(wifiListFileName.c_str() , "r");
        bool isAppendRn = f.size() != 0;

        Println(getWorkName() , "获取到的文件内容:" + string(f.readString().c_str()));
        f = LittleFS.open(wifiListFileName.c_str() , "a");
        if (isAppendRn) {
            f.write("\n");
        }

        f.write(ssid.c_str()); 
        f.write(splitStr.c_str());
        f.write(pwd.c_str());
        f.flush();
        f = LittleFS.open(wifiListFileName.c_str() , "r");

        Println(getWorkName() , "获取到的文件内容:" + string(f.readString().c_str()));

        f.close();
        Println(getWorkName() , "添加WiFi完成");

        return true;
    }
    inline string getWorkName() {
        return "WiFiConnectWork";
    }
    inline void initWork(WorkStatus status) {
        status.init(getWorkName());
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
};
