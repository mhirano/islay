//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_ENGINEBASE_H
#define ISLAY_ENGINEBASE_H

#include "AppMsg.h"
#include "Worker.h"

class EngineBase {
protected:
    std::map<std::string, std::shared_ptr<WorkerManager>> workers;
    AppMsgPtr appMsg;
    std::shared_ptr<PUBinder> puBinder;

public:
    EngineBase (AppMsgPtr _appMsg): appMsg(std::move(_appMsg)),
                                    puBinder(std::make_shared<PUBinder>())
    {
    };

    virtual ~EngineBase(){
        workers.clear();
    };

    virtual bool run() = 0;
    virtual bool reset() = 0;

    bool terminateWorker(const std::string name) {
        if (!isWorkerExist(name)) {
            SPDLOG_WARN("Worker not found: {}", name);
            return false;
        }
        return workers.at(name)->terminate();
    }

    bool terminateAll(){
        for (auto &[name, worker]: workers) worker->terminate();
        return true;
    }

    template <class T>
    bool registerWorker(std::string name){
        if(isWorkerExist(name)){
            SPDLOG_WARN("Worker already exists.");
            return true;
        }
        auto wm = WorkerManager::createWorkerManager<T>(name, puBinder, appMsg);
        auto [t, b] = workers.try_emplace(name, wm);
        return b;
    };

    bool isWorkerExist(const std::string name){
        return workers.count(name) != 0;
    }

    bool runWorker(std::string name, std::shared_ptr<void> data = nullptr) {
        if(!isWorkerExist(name)){
            SPDLOG_WARN("Worker not found: {}", name);
            return false;
        }
        return workers.at(name)->runWorker(data);
    }

    bool runWorkerWithCpuBinding(std::string name, std::shared_ptr<void> data = nullptr) {
        if(!isWorkerExist(name)){
            SPDLOG_WARN("Worker not found: {}", name);
            return false;
        }
        return workers.at(name)->runWorkerCpuBinded(data);
    }

    bool resetWorker(std::string name) {
        if(!isWorkerExist(name)){
            SPDLOG_WARN("Worker not found: {}", name);
            return false;
        }
        workers.at(name)->status.store(WORKER_STATUS::IDLE);
        return workers.at(name)->reset();
    }

    WORKER_STATUS getWorkerStatus(std::string name){
        if(!isWorkerExist(name)){
            SPDLOG_WARN("Worker not found: {}", name);
            return WORKER_STATUS::NOT_EXIST;
        }
        return workers.at(name)->getStatus();
    }

    std::vector<std::string> getWorkerList(){
        std::vector<std::string> names;
        for (auto&[name, worker]: workers) {
            names.emplace_back(name);
        }
        return std::move(names);
    }

    /// TODO: check if this works
    // Delete worker if idle
    bool deleteWorker(std::string name){
        // check if there exists worker named 'name'
        if(!isWorkerExist(name)){
            SPDLOG_WARN("Worker not found: {}", name);
            return false;
        }

        // Delete worker if idle
        if( getWorkerStatus(name) == WORKER_STATUS::IDLE) {
            workers.erase(workers.find(name));
            SPDLOG_INFO("Worker deleted: {}", name);
        }

        return true;
    }

    // Delete all workers in idle
    bool deleteAllWorker(){
        auto workerNames = getWorkerList();
        for (auto& workerName: workerNames) {
            deleteWorker(workerName);
        }
        return true;
    }

    /**
     * PU
     */
    int getPuIfBinded(std::string workerName){
        return puBinder->getPuIfBinded(workerName);
    };

};

#endif //ISLAY_ENGINEBASE_H
