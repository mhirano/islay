//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_WORKER_H
#define ISLAY_WORKER_H

#include <utility>
#include <thread>
#include <atomic>
#include <iostream>
#include <memory>

#include <typeinfo>

#ifdef __GNUC__
#include <cxxabi.h> // for abi::__cxa_demangle
#endif

#include "AppMsg.h"
#include "Logger.h"
#include "Config.h"
#include "PUBinder.h"

/**
 * @brief Status list of worker
 *   IDLE: The worker is idle
 *   RUNNING: The worker is running
 *   TERMINATE_REQUESTED: The worker is running but termination requested
 *   JOINABLE: The worker is waiting its thread to be joined
 *
 */
enum class WORKER_STATUS {NOT_EXIST = 0, IDLE = 1, RUNNING = 2, TERMINATE_REQUESTED = 3, JOINABLE = 4};

struct WorkerStatusMessenger : public MsgData {
    WORKER_STATUS workerStatus = WORKER_STATUS::IDLE;
};

/**
 * @brief Base class of worker
 *   All workers implemented by user should publicly inherit this class.
 *   See WorkerSample.h.
 */
class WorkerManager;

class WorkerBase {
protected:
    AppMsgPtr appMsg;
    InterThreadMessenger<WorkerStatusMessenger> *workerStatusMessenger;
    std::weak_ptr<WorkerManager> wm;

public:
    explicit WorkerBase(std::weak_ptr<WorkerManager> _wm, AppMsgPtr _appMsg):
            wm(std::move(_wm)), appMsg(std::move(_appMsg)), workerStatusMessenger(new InterThreadMessenger<WorkerStatusMessenger>) {};
    virtual ~WorkerBase(){
        delete workerStatusMessenger;
    };

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

    bool requestCpuBind(
            std::string workerName, std::thread::native_handle_type thread, std::thread::id id
    ) ;
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
 *         registerWorker<WorkerSample>("WorkerSample");
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
class WorkerManager: public std::enable_shared_from_this<WorkerManager>{
public:
    std::thread thisThread;
    std::string workerName;
    std::atomic<WORKER_STATUS> status;
    std::shared_ptr<WorkerBase> t;
    std::weak_ptr<PUBinder> puBinder;

private:
    /**
     * @brief Constructor of WorkerManager
     * User may want to override this function to specify how to terminate the workersers
     *
     * @param _workerName Name of the worker.
     * @param _appMsg Application Messenger. Primary used for inter-thread communication.
     *
     */
    explicit WorkerManager() { };

    template<class T>
    void init(std::string _workerName, std::weak_ptr<PUBinder> _puBinder, AppMsgPtr _appMsg ){
        workerName = std::move(_workerName);
        status = WORKER_STATUS::IDLE;
        puBinder = std::move(_puBinder);
        t = std::make_shared<T>(this->shared_from_this(), _appMsg);
    }

public:
    template <class T>
    static std::shared_ptr<WorkerManager> createWorkerManager(
            std::string _workerName, std::weak_ptr<PUBinder> _puBinder, AppMsgPtr _appMsg
    ) {
        struct OBJ: WorkerManager{};
        auto sp = std::make_shared<OBJ>();
//        auto sp = std::make_shared<WorkerManager>(); // private constructor can not be accessed.
        sp->template init<T>(_workerName, _puBinder, _appMsg);
        return sp;
    }

    /**
     * Move constructor
     * @tparam U Worker class name to be moved
     * @param mg moved instance of WorkerManager
     */
    explicit WorkerManager(WorkerManager&& mg){
//        SPDLOG_DEBUG("Move constructor of WorkerManager launched");
        workerName = std::move(mg.workerName);
        thisThread = std::move(mg.thisThread);
        status.store(mg.status.load());
        t = std::move(mg.t);
        puBinder = std::move(mg.puBinder);
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
        if (!thisThread.joinable()){ return false; }
        thisThread.join();
        // Unregister the binding
        if(puBinder.lock()->unbind(workerName))
            SPDLOG_DEBUG("Worker unbinded: {}", workerName);
        SPDLOG_DEBUG("***********************RESET {}**********************", workerName);
        return true;
    };

    /**
     * @brief Runs worker in the thread managed by WorkerManager
     * TODO: data should be thread protected.
     */
    bool runWorker(std::shared_ptr<void> data = nullptr){
        if (status.load() == WORKER_STATUS::IDLE) {
            thisThread = std::thread ([this, data] {
                SPDLOG_INFO("Worker launched: {}", workerName);
                status.store(WORKER_STATUS::RUNNING);
                t->run(data);
                status.store(WORKER_STATUS::JOINABLE);
                SPDLOG_INFO("Worker completed: {}", workerName);
            });
        } else {
            SPDLOG_INFO("{} is already running", workerName);
        }
        return true;
    }

    bool runWorkerCpuBinded(std::shared_ptr<void> data = nullptr){
        if (status.load() == WORKER_STATUS::IDLE) {
            thisThread = std::thread([this, data] {
                unsigned int logical_id = puBinder.lock()->bindThread(
                        workerName, thisThread.native_handle(), thisThread.get_id());
                if(logical_id != -1){
                    SPDLOG_DEBUG("{} binded to PU #{} (thread id:{})",
                                workerName, logical_id, id_to_uint(thisThread.get_id())
                    );
                } else {
                    SPDLOG_WARN("Failed to bind the thread to a PU. No vacant PUs.");
                }
                SPDLOG_INFO("Worker launched: {}", workerName);
                status.store(WORKER_STATUS::RUNNING);
                t->run(data);
                status.store(WORKER_STATUS::JOINABLE);
                SPDLOG_INFO("Worker completed: {}", workerName);
            });
        } else {
            SPDLOG_INFO("{} is already running", workerName);
        }
        return true;
    }
};


#endif //ISLAY_WORKER_H
