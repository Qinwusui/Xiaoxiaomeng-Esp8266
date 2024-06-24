#pragma once
#include <work/Work.hpp>
#include <work/WorkStatus.hpp>
#include <log/Logger.hpp>
#include <LittleFS.h>
//定义一个文件系统初始化任务
using namespace std;
class FileSystemInitWork :public Work , public Logger {

public:
    inline string getWorkName() {
        return "FileSystemInitWork";
    }

    inline void initWork(WorkStatus status) {
        status.init(getWorkName());
        if (!LittleFS.begin()) {
            status.error(getWorkName() , "初始化文件系统失败");
            return;
        }
        File f = LittleFS.open("/" , "r");
        auto file = f.openNextFile();
        Println(getWorkName() , "当前Flash中存在的所有文件：");
        while (file) {
            Println(getWorkName() , file.fullName());
            file = f.openNextFile();
        }
        status.finished(getWorkName());

    }
    inline void onLoop() {

    }
};
