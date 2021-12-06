//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#include "Engine.h"
#include "AppMsg.h"
#include "Config.h"
#include "Logger.h"
#include "WorkerTest.h"



template <class T>
WorkerManager<T>::WorkerManager(std::string _workerName)
        :workerName(std::move(_workerName)),
         status(WORKER_STATUS::IDLE),
         t(std::move(std::make_shared<T>()))
{
    SPDLOG_DEBUG("Construct WorkerManager(std::string&& _workerName)");
    std::string demangledClassName = abi::__cxa_demangle(typeid(T).name(), 0, 0, nullptr);
    SPDLOG_DEBUG("Demangled worker class name of constructed WorkerManager: {}", demangledClassName);
}

template<class T>
WorkerManager<T>::WorkerManager(std::string _workerName, AppMsgPtr _appMsg):
        workerName(std::move(_workerName)),
        status(WORKER_STATUS::IDLE),
        t(std::move(std::make_shared<T>(_appMsg)))
{
    SPDLOG_DEBUG("Construct WorkerManager(std::string&& _workerName, AppMsgPtr _appMsg)");
    std::string demangledClassName = abi::__cxa_demangle(typeid(T).name(), 0, 0, nullptr);
    SPDLOG_DEBUG("Demangled worker class name of constructed WorkerManager: {}", demangledClassName);
}

bool EngineOffline::run() {
    return true;
}

bool EngineOffline::runTest(){

    // Register if worker not exist
    if(!isWorkerExist("Test")){
        registerWorkerWithAppMsg<TestWithAppMsg>("Test");
    }

    if(workers.at("Test").getStatus() == WORKER_STATUS::IDLE){
        std::shared_ptr<int> hoge(new int(123));
        runWorker("Test", hoge);
    }
    return true;
}
