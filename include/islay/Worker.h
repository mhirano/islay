//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_WORKER_H
#define ISLAY_WORKER_H

#include <utility>
#include <thread>
#include <atomic>
#include <iostream>

#include <typeinfo>

#ifdef __GNUC__
#include <cxxabi.h> // for abi::__cxa_demangle
#endif

#include "AppMsg.h"
#include "Logger.h"
#include "Config.h"


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
 *   See WorkerSample.h.
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
 *         registerWorkerWithAppMsg<WorkerSampleWithAppMsg>("WorkerSampleWithAppMsg");
 *       [without application messenger]
 *         registerWorker<WorkerSample>("WorkerSample");
 *   To run and reset a worker:
 *       [without a paremeter]
 *			runWorker("WorkerSample");
 *          resetWorker("WorkerSample");
 *       [with a parameter]
 *          int hoge = 1;
 *          runWorker("WorkerSampleWithParameter", &hoge);
 *          resetWorker("WorkerSampleWithParameter");
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
    explicit WorkerManager(std::string _workerName)
            :workerName(std::move(_workerName)),
             status(WORKER_STATUS::IDLE),
             t(std::move(std::make_shared<T>()))
    {
//        SPDLOG_DEBUG("Construct WorkerManager(std::string&& _workerName)");
//        std::string demangledClassName = abi::__cxa_demangle(typeid(T).name(), 0, 0, nullptr);
//        SPDLOG_DEBUG("Demangled worker class name of constructed WorkerManager: {}", demangledClassName);
    }

    explicit WorkerManager(std::string _workerName, AppMsgPtr _appMsg)
            : workerName(std::move(_workerName)),
              status(WORKER_STATUS::IDLE),
              t(std::move(std::make_shared<T>(_appMsg))) {
//        SPDLOG_DEBUG("Construct WorkerManager(std::string&& _workerName, AppMsgPtr _appMsg)");
//        std::string demangledClassName = abi::__cxa_demangle(typeid(T).name(), 0, 0, nullptr);
//        SPDLOG_DEBUG("Demangled worker class name of constructed WorkerManager: {}", demangledClassName);
    }

    /**
     * Move constructor
     * @tparam U Worker class name to be moved
     * @param mg moved instance of WorkerManager
     */
    template <class U>
    explicit WorkerManager(WorkerManager<U>&& mg){
//        SPDLOG_DEBUG("Move constructor of WorkerManager launched");
        workerName = std::move(mg.workerName);
        thisThread = std::move(mg.thisThread);
        status.store(mg.status.load());
        t = std::move(mg.t);
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
            t->requestTerminate();
            SPDLOG_DEBUG("***********************TERMINATING {}**********************", workerName);
        }
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


#endif //ISLAY_WORKER_H
