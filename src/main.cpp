#include <main.hpp>
Logger logger;
Scheduler sc;
// 创建一个初始化全局变量的任务
Task tInitAllVariable(TASK_IMMEDIATE , TASK_ONCE , &initAll , &sc);
// 创建一个连接WiFi任务
Task tConnect(&connectWiFiTask , &sc);
// 创建一个时间校准任务
Task tInitTimeClient(TASK_IMMEDIATE , TASK_ONCE , &timeClientInit , &sc);
// 创建一个时间更新任务
Task tUpdateTime(&timeUpdate , &sc);
WiFiConnectWork* wiFiConnectWork = NULL;
TimeClientWork* timeClientWork = NULL;
// 初始化函数
void setup() {
    Serial.begin(9600);
    Serial.println();
    Serial.flush();
    // 初始化任务

    //启动变量初始化任务
    tInitAllVariable.enable();
    //变量初始化任务完成后，启动WiFi连接任务
    tConnect.waitFor(
        tInitAllVariable.getInternalStatusRequest() ,
        5 * TASK_SECOND ,
        TASK_FOREVER
        );
    // 时间初始化任务需要等待WiFi连接任务完成后再开始
    tInitTimeClient.waitFor(tConnect.getInternalStatusRequest());
    //时间更新任务需要等待时间初始化任务完成后再开始
    tUpdateTime.waitFor(
        tInitTimeClient.getInternalStatusRequest() ,
        TASK_SECOND ,
        TASK_FOREVER
        );
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
    tInitAllVariable.getInternalStatusRequest()->signalComplete();
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
