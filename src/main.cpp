#include <main.hpp>
Logger logger;
Scheduler sc;
// 创建一个连接WiFi任务
Task tConnect(&connectWiFiTask , &sc);
// 创建一个时间校准任务
Task tInitTimeClient(TASK_IMMEDIATE , TASK_ONCE , &timeClientInit , &sc);
// 创建一个时间更新任务
Task tUpdateTime(TASK_SECOND , TASK_FOREVER , &timeUpdate , &sc);
// 创建一个web Server任务
Task tCreateWebServer(TASK_IMMEDIATE , TASK_ONCE , &createWebServer , &sc);
WiFiConnectWork* wiFiConnectWork = NULL;
TimeClientWork* timeClientWork = NULL;
WebServerWork* webServerWork = NULL;
FileSystemInitWork* fileSystemInitWork = NULL;
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
    tInitTimeClient.enable();
    //时间更新任务
    tUpdateTime.enable();
    //启动创建webserver任务
    tCreateWebServer.enable();
}
// 时间更新任务
void timeUpdate() {
    if (timeClientWork) {
        timeClientWork->onLoop();
    }
}
// 时间初始化任务
void timeClientInit() {
    timeClientWork->initWork(
        {
            .init = [] (std::string name) {
                logger.Println(name,"开始执行时间校准任务");
            },
            .finished = [] (std::string name) {
                logger.Println(name,"时间校准任务完成");
            }
        }
    );
}
void initAll() {
    logger = Logger();
    logger.Println("Main" , "开始初始化所有全局变量");
    wiFiConnectWork = new WiFiConnectWork("wusui_2.4G" , "Qinsansui233...");
    timeClientWork = new TimeClientWork();
    webServerWork = new WebServerWork();
    fileSystemInitWork = new FileSystemInitWork();
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
void createWebServer() {
    webServerWork->initWork(
        {
            .init = [] (std::string name) {
                logger.Println(name,"开始执行创建webServer任务");
            },
            .running = [] (std::string name) {
                logger.Println(name,"调用");
            },
            .finished = [] (std::string name) {
                logger.Println(name,"初始化WebServer完成");
            },
            .error = [] (std::string name,std::string reason) {
                logger.Println(name,reason);
            },
        }
    );
}
// 连接WiFi任务
void connectWiFiTask() {
    logger.Println("Main" , "执行连接WiFi任务");
    wiFiConnectWork->initWork({ .finished = [] (std::string name) {
        logger.Println(name , "任务结束");
        tConnect.getInternalStatusRequest()->signalComplete();
        tConnect.disable();
        } });
}

// 循环函数
void loop() { sc.execute(); }
