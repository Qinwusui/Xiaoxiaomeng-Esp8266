#pragma once
#include <string>
using namespace std;
//定义各个任务状态返回的函数

typedef std::function<void(std::string name)> OnWorkInit;
typedef std::function<void(std::string name)> OnWorkRunning;
typedef std::function<void(std::string name)> OnWorkFinished;
typedef std::function<void(std::string name , std::string reason)> OnWorkError;

//定义任务状态
struct WorkStatus {
    OnWorkInit init = [] (std::string name) {};
    OnWorkRunning running = [] (std::string name) {};
    OnWorkFinished finished = [] (std::string name) {};
    OnWorkError error = [] (std::string name , std::string reason) {};
};
// class WorkStatus {
// public:
// OnWorkInit init;
// OnWorkRunning running;
// OnWorkFinished finished;
// OnWorkError error;
// };
