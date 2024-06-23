#include <main.hpp>
Logger logger;
Scheduler sc;
// 创建一个让所有任务循环执行的任务
Task tLoopWork(200 , TASK_FOREVER , &onWorkLoop , &sc);
// 创建一个初始化全局变量的任务
Task tInitAllVariable(TASK_IMMEDIATE , TASK_ONCE , &initAll , &sc);
// 创建一个连接WiFi任务
Task tConnect(&connectWiFiTask , &sc);
// 创建一个更新时间的任务
Task tUpdateTime(TASK_IMMEDIATE , TASK_ONCE , &timeUpdate , &sc);
WiFiConnectWork* wiFiConnectWork = NULL;
// 初始化函数
void setup() {
    Serial.begin(9600);
    Serial.println();
    Serial.flush();
    // 让所有任务循环执行
    tLoopWork.enable();
    // 初始化任务
    tInitAllVariable.enable();
    tConnect.waitFor(tInitAllVariable.getInternalStatusRequest() , 5 * TASK_SECOND ,
        TASK_FOREVER);
// 时间更新任务需要等待WiFi连接任务完成后再开始
    tUpdateTime.waitFor(tConnect.getInternalStatusRequest());
}
// 时间更新任务
void timeUpdate() {

}
void onWorkLoop() {
    if (wiFiConnectWork) {
        wiFiConnectWork->onLoop();
    }
}
void initAll() {
    logger = Logger();
    logger.Println("Main" , "开始初始化所有全局变量");
    wiFiConnectWork = new WiFiConnectWork("wusui_2.4G" , "Qinsansui233...");
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
