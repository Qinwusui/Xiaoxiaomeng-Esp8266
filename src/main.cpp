#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <main.hpp>
Logger logger;
Scheduler sc;
// 创建一个连接WiFi任务
AsyncWebServer server(80);  // 在端口80上创建服务器
const int relayPin = D0;    // 继电器连接的GPIO pin
Task tConnect(TASK_SECOND , TASK_FOREVER , &connectWiFiTask , &sc);
//创建一个WiFi监听任务
Task tWiFiListener(&wifiStatusListener , &sc);
// 创建一个时间校准任务
Task tInitTimeClient(TASK_IMMEDIATE , TASK_ONCE , &timeClientInit , &sc);
// 创建一个时间更新任务
Task tUpdateTime(TASK_SECOND , TASK_FOREVER , &timeUpdate , &sc);
// 创建一个web Server任务
Task tCreateWebServer(TASK_IMMEDIATE , TASK_ONCE , &createWebServer , &sc);
//初始化对象
WiFiConnectWork* wiFiConnectWork = NULL;
TimeClientWork* timeClientWork = NULL;
WebServerWork* webServerWork = NULL;
FileSystemInitWork* fileSystemInitWork = NULL;
WiFiStatusListenerWork* wifiStatusListenerWork = NULL;
//定义参考https://blog.csdn.net/dpjcn1990/article/details/92831760
U8G2_SSD1306_128X64_NONAME_F_HW_I2C  u8g2(U8G2_R0 , U8X8_PIN_NONE , 14 , 13);
// 网页内容
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Relay Control</title>
    <style>
    button {
      width: 200px;
      height: 50px; 
      font-size: 20px; 
      margin: 20px;
    }
  </style>
</head>
<body>
  <button onclick="toggleRelay()">press</button>
  <script>
    function toggleRelay() {
      fetch('/toggle')
        .then(response => response.text())
        .then(state => console.log("Relay state: " + state))
        .catch(console.error);
    }
  </script>
</body>
</html>
)rawliteral";

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
    //启动WiFi监听任务
    tWiFiListener.waitFor(tConnect.getInternalStatusRequest() , TASK_SECOND , TASK_FOREVER);
}
//WiFi监听任务
void wifiStatusListener() {
    if (wifiStatusListenerWork) {
        WiFiStatus* status = wifiStatusListenerWork->onLoop<WiFiStatus*>();
        String s = String(status->length);
        logger.Println("wifiStatusListener" , status->ssid + " " + string(status->connected?"已连接":"已断开") + " " + s.c_str());
    }

}
//显示文本到屏幕
void showText(int x , int y , const char* text) {
    u8g2.setCursor(x , y);
    u8g2.print(text);
    u8g2.sendBuffer();
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
    wiFiConnectWork = new WiFiConnectWork("CIA-2.4G" , "204fastest");
    timeClientWork = new TimeClientWork();
    webServerWork = new WebServerWork();
    fileSystemInitWork = new FileSystemInitWork();
    wifiStatusListenerWork = new WiFiStatusListenerWork();
    initScreen();

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
    pinMode(relayPin , OUTPUT);  // 设置继电器pin为输出模式
    digitalWrite(relayPin , LOW); // 默认关闭继电器

    server.on("/" , HTTP_GET , [] (AsyncWebServerRequest* request) {
        request->send_P(200 , "text/html" , index_html);
        });

    server.on("/toggle" , HTTP_GET , [] (AsyncWebServerRequest* request) {
        digitalWrite(relayPin , !digitalRead(relayPin));  // 切换继电器状态
        request->send(200 , "text/plain" , digitalRead(relayPin) ? "ON" : "OFF");
        });

    server.begin();  // 启动服务器
    logger.Println("WebServer" , "服务器已启动");
}

// 连接WiFi任务
void connectWiFiTask() {
    logger.Println("Main" , "执行连接WiFi任务");
    wiFiConnectWork->initWork(
        {
            .finished = [] (std::string name) {
                logger.Println(name , "任务结束");
                logger.Println(name,"IP:");
                std::string ip = WiFi.localIP().toString().c_str();
                logger.Println(name,ip);
                showText(128 - u8g2.getUTF8Width(ip.c_str()) , 64,ip.c_str());
                tConnect.getInternalStatusRequest()->signalComplete();
                tConnect.disable();
             }
        });
}

// 循环函数
void loop() { sc.execute(); }
