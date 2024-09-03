#pragma once
#include <serial/SerialTask.hpp>
#include <SoftwareSerial.h>
using namespace std;
class ReplaySerialWork :public SerialWork {
private:
    int8_t RX , TX;
    SoftwareSerial* serial;
public:
    ReplaySerialWork(int8_t RX , int8_t TX) {
        this->RX = RX;
        this->TX = TX;
        this->serial = new SoftwareSerial(this->RX , this->TX);

        pinMode(RX , INPUT);
        pinMode(TX , OUTPUT);
        serial->begin(9600);
    }
    inline string  getWorkName() {
        return "ReplaySerialWork";
    }
    inline void initWork(WorkStatus status) {
        status.finished(getWorkName());
    }
    inline void sendString(String s) {
        serial->print(s);
        serial->flush();
        serial->flush();
        serial->flush();
    }
    template <typename T>
    inline T onLoop() {
        if (serial->available() > 0) {
            return serial->readStringUntil('\n');
        }
        return "";
    }
};
