//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_ENGINE_H
#define ISLAY_ENGINE_H

#include <utility>
#include <thread>
#include "AppMsg.h"

class Engine {
public:
    Engine (AppMsgPtr _appMsg): appMsg(std::move(_appMsg)){};
    virtual bool run() = 0;
    virtual bool reset() = 0;
//    bool terminate(){
//        if (worker.joinable()) {
//            worker.join();
//            return true;
//        }
//    }

    AppMsgPtr appMsg;
};

class EngineOffline: public Engine{
    std::thread worker;
public:
    EngineOffline(AppMsgPtr _appMsg): Engine(std::move(_appMsg)){};
    ~EngineOffline(){
        if(worker.joinable())
            worker.join();
    }
    bool run() override;
    bool reset() override;
};

#endif //ISLAY_ENGINE_H
