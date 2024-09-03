#pragma once
#include <WebSocketsClient.h>
#include <work/Work.hpp>
#include <work/WorkStatus.hpp>
#include <log/Logger.hpp>
typedef function<void(std::string payload)> onWsEvent;



class WebSocketClient :public Work , public Logger {
private:
    WebSocketsClient client;
    int port;
    String host , path , auth;
public:
    WebSocketClient(String host , int port , String path , String auth);
    inline string getWorkName();
    inline void initWork(WorkStatus status);
    template<typename T>
    inline T onLoop();
    inline void loop();
    void onWebSocketEvent(onWsEvent event) {
        client.onEvent(
            [=] (WStype_t type , uint8_t* payload , size_t length) {
                if (type == WStype_TEXT) {
                    string s = string((char*) payload);
                    event(s);
                    Println(getWorkName() , "已收到服务器的消息：" + string(s));

                    return;
                }
                if (type == WStype_DISCONNECTED) {
                    Println(getWorkName() , "WebSocket连接已断开");
                    event("ws_Closed");
                    return;
                }
            }
        );
    }
    void sendText(String message , bool isNoReply = false) {
        String s = isNoReply?message:("isNoReply:" + message);
        client.sendTXT(s);

    }
};

WebSocketClient::WebSocketClient(String host , int port , String path , String auth) {
    this->auth = auth;
    this->host = host;
    this->port = port;
    this->path = path;
}
inline string WebSocketClient::getWorkName() {
    return "WebSocketClient";
}
inline void WebSocketClient::initWork(WorkStatus status) {
    status.init(getWorkName());
    client.begin(host , port , path);
    client.setAuthorization("wusui" , auth.c_str());
    client.setExtraHeaders(string("Device: ").append(String(ESP.getChipId()).c_str()).c_str());
    client.setReconnectInterval(2000);
    client.enableHeartbeat(1000 , 2000 , 200);
    status.finished(getWorkName());
}
void WebSocketClient::loop() {
    client.loop();
}
template <typename T>
inline T WebSocketClient::onLoop() {

    return client.isConnected();
}
