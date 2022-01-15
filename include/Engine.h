//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_ENGINE_H
#define ISLAY_ENGINE_H

#include <utility>
#include <thread>
#include <atomic>
#include <iostream>
#include "AppMsg.h"
#include "Logger.h"
#include "InterThreadMessenger.hpp"

#include <typeinfo>
#include <cxxabi.h> // for abi::__cxa_demangle

/**
 * @brief Status list of worker
 *   IDLE: The worker is idle
 *   RUNNING: The worker is running
 *   TERMINATE_REQUESTED: The worker is running but termination requested
 *   JOINABLE: The worker is waiting its thread to be joined
 *
 */
enum class WORKER_STATUS {IDLE = 0, RUNNING = 1, TERMINATE_REQUESTED = 2, JOINABLE = 3};

struct WorkerStatusMessenger : public MsgData {
    WORKER_STATUS workerStatus = WORKER_STATUS::IDLE;
};

/**
 * @brief Base class of worker
 *   All workers implemented by user should publicly inherit this class.
 *   See Test.h.
 */
class WorkerBase {
protected:
    AppMsgPtr appMsg;
    InterThreadMessenger<WorkerStatusMessenger> *workerStatusMessenger;
public:
    explicit WorkerBase():
            workerStatusMessenger(new InterThreadMessenger<WorkerStatusMessenger>) {};
    explicit WorkerBase(AppMsgPtr _appMsg):
        appMsg(_appMsg),
        workerStatusMessenger(new InterThreadMessenger<WorkerStatusMessenger>) {};
    virtual bool run(const std::shared_ptr<void> data) = 0;
    bool requestTerminate(){
        auto dm = workerStatusMessenger->prepareMsg();
        dm->workerStatus = WORKER_STATUS::TERMINATE_REQUESTED;
        workerStatusMessenger->send();
        return true;
    }
    bool checkIfTerminateRequested(){
        auto md = workerStatusMessenger->receive();
        if (md != nullptr) {
            if(md->workerStatus == WORKER_STATUS::TERMINATE_REQUESTED){
                return true;
            }
        }
        return false;
    };
};

/**
 * @brief Worker manager
 *   This class manages the behavior of the worker, including
 *   - hold worker instance
 *   - thread management
 *   - status management
 *
 *   Users should use this manager to control workers.
 *   To register a worker:
 *       [with application messenger]
 *         registerWorkerWithAppMsg<TestWithAppMsg>("TestWith");
 *       [without application messenger]
 *         registerWorker<Test>("Test");
 *   To run and reset a worker:
 *       [without a paremeter]
 *			runWorker("Test");
 *          resetWorker("Test");
 *       [with a parameter]
 *          int hoge = 1;
 *          runWorker("TestWith", &hoge);
 *          resetWorker("TestWith");
 *
 *   See EngineOffline::runTest().
 */
template <class T=WorkerBase>
class WorkerManager{
public:
    std::thread thisThread;
    std::string workerName;
    std::atomic<WORKER_STATUS> status;
    std::shared_ptr<WorkerBase> t;

    /**
     * @brief Constructor of WorkerManager
     * User may want to override this function to specify how to terminate the workersers
     *
     * @param _workerName Name of the worker.
     * @param _appMsg Application Messenger. Primary used for inter-thread communication.
     *
     */
    explicit WorkerManager(std::string _workerName);
    explicit WorkerManager(std::string _workerName, AppMsgPtr _appMsg);

    /**
     * Move constructor
     * @tparam U Worker class name to be moved
     * @param mg moved instance of WorkerManager
     */
    template <class U>
    explicit WorkerManager(WorkerManager<U>&& mg){
        SPDLOG_DEBUG("Move constructor of WorkerManager launched");
        workerName = std::move(mg.workerName);
        thisThread = std::move(mg.thisThread);
        status.store(mg.status.load());
        t = std::move(mg.t);
//		int hoge;
//		std::cout << "mg.t:" << abi::__cxa_demangle(typeid(mg.t).name(), 0, 0, &hoge) << std::endl;
//		std::cout << "t:" << abi::__cxa_demangle(typeid(t).name(), 0, 0, &hoge) << std::endl;
//		std::cout << "U:" << abi::__cxa_demangle(typeid(U).name(), 0, 0, &hoge) << std::endl;
    }

    ~WorkerManager(){
        if (thisThread.joinable()) thisThread.join();
    };

    /**
     * @brief Request termination of the worker
     * User may want to override this function to specify how to terminate the workersers
     */
    bool terminate(){
        if (status.load() == WORKER_STATUS::RUNNING){
            status.store(WORKER_STATUS::TERMINATE_REQUESTED);
            SPDLOG_DEBUG("***********************TERMINATING {}**********************", workerName);
        }

        t->requestTerminate();

        return true;
    };

    /**
     * @brief Returns workers status
     */
    WORKER_STATUS getStatus(){ return status.load(); }

    /**
     * @brief Join the thread
     * @return true if the worker is successfully reset
     */
    bool reset(){
        if (thisThread.joinable()){
            thisThread.join();
            SPDLOG_DEBUG("***********************RESET {}**********************", workerName);
        }
        return true;
    };

    /**
     * @brief Runs worker in the thread managed by WorkerManager
     * TODO: data should be thread protected.
     */
    bool runWorker(std::shared_ptr<void> data = nullptr){
        if (status.load() == WORKER_STATUS::IDLE) {
            thisThread = std::thread([this, data] {
                SPDLOG_INFO("{} launched", workerName);
                status.store(WORKER_STATUS::RUNNING);
                t->run(data);
                status.store(WORKER_STATUS::JOINABLE);
                SPDLOG_INFO("{} finished", workerName);
            });
        } else {
            SPDLOG_DEBUG("{} is already running", workerName);
        }
        return true;
    }
};

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

class EngineOffline: public Engine{
public:
    EngineOffline(AppMsgPtr _appMsg): Engine(std::move(_appMsg)){};
    ~EngineOffline(){
        reset();
    }
    bool run() override;
    bool reset() override {
        for(auto& [name, worker]: workers) worker.reset();
    };

};

#endif //ISLAY_ENGINE_H
