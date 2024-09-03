#pragma once
#include <WiFiClientSecure.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
typedef std::function<void(JsonDocument&)> Success;
typedef std::function<void(String)> Failure;
typedef std::function<void()> Requesting;
struct HttpRequestStatus {
    Requesting requesting = [] () {};

    Success success = [] (JsonDocument&) {};
    Failure failure = [] (String) {};
};

class HttpClient {
private:
    WiFiClientSecure https;
    WiFiClient client;
    HTTPClient http;
public:
    HttpClient() {
        https.setInsecure();
    }
    inline void get(String url , HttpRequestStatus status);
};
//GET
inline void HttpClient::get(String url , HttpRequestStatus status) {
    status.requesting();
    if (http.begin(client , https , url)) {
        http.setUserAgent("wusui-" + String(ESP.getChipId()));
        int code = http.GET();
        if (code == 200) {
            String s = http.getString();
            JsonDocument d;
            deserializeJson(d , s);
            status.success(d);
            return;
        }
        status.failure("响应：" + String(code));
    }
    status.failure("响应超时");

}