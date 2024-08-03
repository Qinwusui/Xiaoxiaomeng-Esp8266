#pragma once
using namespace std;
#include <work/Work.hpp>
#include <work/WorkStatus.hpp>
#include <log/Logger.hpp>

class SerialWork :public Work , public Logger {
public:
    SerialWork() {

    }
    inline string getWorkName() {
        return "SerialWork";
    }
    inline void initWork(WorkStatus status) {

    }
    template <typename T>
    inline T onLoop() {

    }
};
