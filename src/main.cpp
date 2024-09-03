#include <main.hpp>
Logger logger;
Scheduler sc;
// 创建一个连接WiFi任务
Task tConnect(TASK_SECOND , TASK_FOREVER , &connectWiFiTask , &sc);
//创建一个WiFi监听任务
Task tWiFiListener(&wifiStatusListener , &sc);
// 创建一个时间校准任务
Task tInitTimeClient(TASK_IMMEDIATE , TASK_ONCE , &timeClientInit , &sc);
// 创建一个时间更新任务
Task tUpdateTime(TASK_SECOND , TASK_FOREVER , &timeUpdate , &sc);
//创建一个串口通信任务
Task tSerialCommunication(80 , TASK_FOREVER , &serialLoop , &sc);
// 创建一个web Server任务
// Task tCreateWebServer(TASK_IMMEDIATE , TASK_ONCE , &createWebServer , &sc);
//创建一个web Socket启动任务
Task tCreateWsClient(&createWsClientTask , &sc);
//创建一个webSocket轮询任务
Task tWsClientLoop(TASK_SECOND , TASK_FOREVER , &wsLoopTask , &sc);
//创建一个wsLoop任务
Task tWsLoop(80 , TASK_FOREVER , &wsLoop , &sc);
//初始化对象
WiFiConnectWork* wiFiConnectWork = NULL;
TimeClientWork* timeClientWork = NULL;
// WebServerWork* webServerWork = NULL;
FileSystemInitWork* fileSystemInitWork = NULL;
WiFiStatusListenerWork* wifiStatusListenerWork = NULL;
//初始化软串口通信
ReplaySerialWork* replaySerialWork = NULL;
//HTTP请求客户端
HttpClient* httpClient = NULL;
//WebSocket客户端
WebSocketClient* wsClient = NULL;
//定义参考https://blog.csdn.net/dpjcn1990/article/details/92831760
//TODO 需要改成其他端口
U8G2_SSD1306_128X64_NONAME_F_HW_I2C  u8g2(U8G2_R0 , U8X8_PIN_NONE , 14 , 13);

// 初始化函数
void setup() {
    Serial.begin(9600);
    Serial.println();
    Serial.flush();
    // 初始化任务

    //启动变量初始化
    initAll();
    //变量初始化任务完成后，启动WiFi连接任务
    tConnect.enable();
    // 时间初始化任务
    // tInitTimeClient.enable();
    //时间更新任务
    // tUpdateTime.enable();
    //串口通信任务
    tSerialCommunication.enable();
    //启动创建webserver任务
    // tCreateWebServer.enable();
    //启动WiFi监听任务
    tWiFiListener.waitFor(tConnect.getInternalStatusRequest() , TASK_SECOND , TASK_FOREVER);
    //webSocket需要在连接网络后尝试建立
    tCreateWsClient.waitFor(tConnect.getInternalStatusRequest() , TASK_SECOND , TASK_ONCE);
    //webSocket轮询任务需要在建立ws连接后尝试启动
    tWsClientLoop.waitFor(tCreateWsClient.getInternalStatusRequest() , TASK_SECOND , TASK_FOREVER);

    tWsLoop.waitFor(tCreateWsClient.getInternalStatusRequest() , 100 , TASK_FOREVER);

}
//ws连接任务
void createWsClientTask() {
    if (wsClient) {
        wsClient->onWebSocketEvent(
            [] (string p) {
                logger.Println(wsClient->getWorkName() , "收到webSocket消息:" + p);
                commandHandler(p.c_str());
            }
        );
        wsClient->initWork(
            {
                .init = [] (string name) {
                        logger.Println(name,"正在建立Ws连接");
                },
                 .finished = [] (string name) {
                    logger.Println(name,"发起连接成功");
                },
                .error = [] (string name ,string reason) {
                        logger.Println(name,reason);
                },

            }
        );

    }
    tCreateWsClient.getInternalStatusRequest()->signalComplete();

}
void wsLoop() {
    if (wsClient) {
        wsClient->loop();
    }

}
//WS客户端轮询任务
void wsLoopTask() {
    bool isConnected = wsClient->onLoop<bool>();
    if (!isConnected) {
        logger.Println("wsLoopTask" , "WebSocket已断开连接");
    }



}
//WiFi监听任务
void wifiStatusListener() {
    if (wifiStatusListenerWork) {
        WiFiStatus status = wifiStatusListenerWork->onLoop<WiFiStatus>();
        String s = String(status.length);

    }

}
//显示文本到屏幕
void showText(int x , int y , const char* text) {
    // u8g2.setCursor(x , y);
    // u8g2.print(text);

    // u8g2.sendBuffer();

}
//屏幕初始化任务
void initScreen() {
    // // 初始化显示屏
    u8g2.begin();
    u8g2.enableUTF8Print();
    u8g2.setFont(u8g2_font_wqy12_t_gb2312b);
}

// 时间更新任务
void timeUpdate() {
    if (timeClientWork) {
        std::string  time = (std::string) timeClientWork->onLoop<string>();
        showText(0 , 13 , time.c_str());
    }
}
//串口通信任务
void serialLoop() {
    if (replaySerialWork) {
        String s = replaySerialWork->onLoop<String>();
        if (s == "") {
            return;
        }
        logger.Println("serialLoop" , string("发送消息").append(s.c_str()));

        if (wsClient) {
            wsClient->sendText(s);
        }
        // commandHandler(s);
    }

}
//命令处理
void commandHandler(String command) {
    if (replaySerialWork) {
        replaySerialWork->sendString(command);
        logger.Println("HTTP" , string("收到响应：").append(command.c_str()));
    }
}
// 时间初始化任务
void timeClientInit() {
    timeClientWork->initWork(
        {
            .init = [] (std::string name) {
                logger.Println(name, "开始执行时间校准任务");
            },
            .finished = [] (std::string name) {
                logger.Println(name, "时间校准任务完成");
            }
        }
    );
}
void initAll() {
    logger = Logger();

    logger.Println("Main" , "开始初始化所有全局变量");
    httpClient = new HttpClient();
    wsClient = new WebSocketClient("192.168.123.12" , 3456 , "/ws" , "Qinsansui233...");
    fileSystemInitWork = new FileSystemInitWork();
    wiFiConnectWork = new WiFiConnectWork("wusui_2.4G" , "Qinsansui233...");
    timeClientWork = new TimeClientWork();
    // webServerWork = new WebServerWork();
    wifiStatusListenerWork = new WiFiStatusListenerWork();
    replaySerialWork = new ReplaySerialWork(13 , 14);
    replaySerialWork->initWork(
        {
            .finished = [] (std::string name) {
                logger.Println(name,"创建串口通信任务完成");
            }
        }
    );
    // initScreen();

    fileSystemInitWork->initWork(
        {
            .init = [] (std::string name) {
                    logger.Println(name,"初始化");
            },
            .finished = [] (std::string name) {
                logger.Println(name,"初始化完成");
            },
        }
    );

}
//创建webServer任务
// void createWebServer() {
//     webServerWork->initWork(
//         {
//             .init = [] (std::string name) {
//                 logger.Println(name,"开始执行创建webServer任务");
//             },
//             .running = [] (std::string name) {
//                 logger.Println(name,"调用");
//             },
//             .finished = [] (std::string name) {
//                 logger.Println(name,"初始化WebServer完成");
//             },
//             .error = [] (std::string name,std::string reason) {
//                 logger.Println(name,reason);
//             },
//         }
//     );
// }
// 连接WiFi任务
void connectWiFiTask() {
    wiFiConnectWork->initWork(
        {
            .finished = [] (std::string name) {
                logger.Println(name , "任务结束");
                logger.Println(name,"IP:");
                std::string ip = WiFi.localIP().toString().c_str();
                logger.Println(name,ip);
                // showText(128 - u8g2.getUTF8Width(ip.c_str()) , 64,ip.c_str());
                tConnect.getInternalStatusRequest()->signalComplete();
                tConnect.disable();
             }
        });
}

// 循环函数
void loop() { sc.execute(); }
