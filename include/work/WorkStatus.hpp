#pragma once
#include <string>
using namespace std;
//定义各个任务状态返回的函数

typedef std::function<void(std::string name)> OnWorkInit;
typedef std::function<void(std::string name)> OnWorkRunning;
typedef std::function<void(std::string name)> OnWorkFinished;
typedef std::function<void(std::string name)> OnWorkError;

//定义任务状态
struct WorkStatus
{
    OnWorkInit init =[](std::string){};
    OnWorkRunning running = [] (std::string) {};
    OnWorkFinished finished = [] (std::string) {};
    OnWorkError error = [] (std::string) {};
};
