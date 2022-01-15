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
    /*
     * Register a worker with its name
     */
    registerWorkerWithAppMsg<TestWithAppMsg>("Test");

    /*
     * You can pass a variable to the worker as a shared pointer.
     */
    std::shared_ptr<int> hoge(new int(123));

    /*
     * Run the worker
     */
    runWorker("Test", hoge);

    return true;
}
