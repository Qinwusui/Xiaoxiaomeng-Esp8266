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
    WiFiClientSecure client;
    HTTPClient http;
public:
    HttpClient() {
        client.setInsecure();
    }
    inline void get(String url , HttpRequestStatus status);
};
//GET
inline void HttpClient::get(String url , HttpRequestStatus status) {
    status.requesting();
    http.addHeader("Device-Type" , "Esp8266-" + ESP.getChipId());
    if (http.begin(client , url)) {
        int code = http.GET();
        if (code != 200) {
            status.failure("状态码" + code);
            return;
        }
        String body = http.getString();
        Serial.println(body);
        JsonDocument document;
        deserializeJson(document , body);
        status.success(document);
        http.end();
    }

}