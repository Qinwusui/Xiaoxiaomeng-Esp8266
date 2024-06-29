#pragma once
#include <functional>
#include <work/WorkStatus.hpp>
using namespace std;
//定义一个工作接口
class Work {

public:
    /**
     * 接口中声明的方法都为virtual纯虚函数
     * 所有实现这个接口的对象都需要重写被virtual修饰的函数
     * inline修饰的函数表示内联函数
     * 编译时会将该函数直接替换到调用点
     * 可以节省函数调用栈的空间和跳转函数的时间
     */
     //定义一个获取任务名方法
    virtual inline std::string getWorkName() = 0;
    //定义一个初始化任务方法，该方法传入一个lambda函数
    virtual inline void initWork(WorkStatus status) = 0;
    //定义一个循环执行的任务方法，这个方法会一直循环执行
    template<typename T>
    inline T onLoop();
};