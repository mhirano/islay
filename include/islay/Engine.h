//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_ENGINE_H
#define ISLAY_ENGINE_H

#include "AppMsg.h"
#include "Worker.h"

class Engine {
protected:
    std::map<std::string, WorkerManager<WorkerBase>> workers;
    AppMsgPtr appMsg;
public:
    Engine (AppMsgPtr _appMsg): appMsg(std::move(_appMsg)){
    };

    virtual ~Engine(){
        workers.clear();
    };

    virtual bool run() = 0;
    virtual bool reset() = 0;
    bool terminate(){
        for (auto &[name, worker]: workers) worker.terminate();
        return true;
    }

    // Register worker
    template <class T>
    bool registerWorker(std::string name){
        auto [t,b] = workers.try_emplace(name, std::move(WorkerManager<T>(name)));
        return b;
    };

    template <class T>
    bool registerWorkerWithAppMsg(std::string name){
        if(isWorkerExist(name)){
            return true;
        }
        auto [t, b] = workers.try_emplace(name, std::move(WorkerManager<T>(name, appMsg)));
        return b;
    };

    bool isWorkerExist(std::string name){
        if(workers.count(name) == 0){
            return false;
        } else {
            return true;
        }
    }

    bool runWorker(std::string name, std::shared_ptr<void> data = nullptr) {
        return workers.at(name).runWorker(data);
    }

    bool resetWorker(std::string name) {
        workers.at(name).status.store(WORKER_STATUS::IDLE);
        return workers.at(name).reset();
    }

    WORKER_STATUS getWorkerStatus(std::string name){
        return workers.at(name).getStatus();
    }

    std::vector<std::string> getWorkerList(){
        std::vector<std::string> names;
        for (auto&[name, worker]: workers) {
            names.emplace_back(name);
        }
        return std::move(names);
    }

};

#endif //ISLAY_ENGINE_H
