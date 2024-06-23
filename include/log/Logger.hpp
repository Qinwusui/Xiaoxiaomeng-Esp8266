#pragma once
#include <Arduino.h>
using namespace std;
//定义一个日志打印类
class Logger {

public:
    virtual inline void Println(string tag , string log) {
        Serial.printf("[%s] %s\n" , tag.c_str() , log.c_str());
        Serial.flush();
    };
};