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
Task tSerialCommunication(TASK_SECOND , TASK_FOREVER , &serialLoop , &sc);
// 创建一个web Server任务
// Task tCreateWebServer(TASK_IMMEDIATE , TASK_ONCE , &createWebServer , &sc);
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
//定义参考https://blog.csdn.net/dpjcn1990/article/details/92831760
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
    // tWiFiListener.waitFor(tConnect.getInternalStatusRequest() , TASK_SECOND , TASK_FOREVER);
}
//WiFi监听任务
void wifiStatusListener() {
    if (wifiStatusListenerWork) {
        WiFiStatus* status = wifiStatusListenerWork->onLoop<WiFiStatus*>();
        String s = String(status->length);
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
        serialCommandHandler(s);
    }

}
//串口命令处理
void serialCommandHandler(String command) {
    if (command == "openLight") {
        httpClient->get("https://www.baidu.com" ,
            {
                .requesting = [] () {
                    logger.Println("HTTP","正在请求");
                },
                .success = [] (JsonDocument& document) {
                    replaySerialWork->sendString("light_Opened");
                    logger.Println("HTTP","请求成功");
                },
                .failure = [] (String reason) {
                    replaySerialWork->sendString("light_Open_failure");

                    logger.Println("HTTP",reason.c_str());
                }
            }
        );
    }
    if (command == "closeLight") {
        httpClient->get("https://www.baidu.com" ,
            {
                .requesting = [] () {
                    logger.Println("HTTP","正在请求");
                },
                .success = [] (JsonDocument& document) {
                    replaySerialWork->sendString("light_Closed");
                    logger.Println("HTTP","请求成功");
                },
                .failure = [] (String reason) {
                    replaySerialWork->sendString("light_Close_failure");

                    logger.Println("HTTP",reason.c_str());
                }
            }
        );
    }
    if (command == "openConditioner") {
        httpClient->get("https://www.baidu.com" ,
            {
                .requesting = [] () {
                    logger.Println("HTTP","正在请求");
                },
                .success = [] (JsonDocument& document) {
                    replaySerialWork->sendString("conditioner_Opened");
                    logger.Println("HTTP","请求成功");
                },
                .failure = [] (String reason) {
                    replaySerialWork->sendString("conditioner_Open_failure");
                    logger.Println("HTTP",reason.c_str());
                }
            }
        );
    }
    if (command == "closeConditioner") {
        httpClient->get("https://www.baidu.com" ,
            {
                .requesting = [] () {
                    logger.Println("HTTP","正在请求");
                },
                .success = [] (JsonDocument& document) {
                    replaySerialWork->sendString("conditioner_Closed");
                    logger.Println("HTTP","请求成功");
                },
                .failure = [] (String reason) {
                    replaySerialWork->sendString("conditioner_Close_failure");

                    logger.Println("HTTP",reason.c_str());
                }
            }
        );
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
