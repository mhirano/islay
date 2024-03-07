//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#include "Engine.h"
#include "WorkerSample.h"

bool Engine::run() {
    return true;
}

bool Engine::runWorkerSample() {
    /**
     * Register a worker with its name
     */
    registerWorker<WorkerSample>("WorkerSample");

    /**
     * You can pass a variable to the worker as a shared pointer.
     */
    std::shared_ptr<int> hoge(new int(123));

    /**
     * Run the worker
     */
    runWorker("WorkerSample", hoge);

    return true;
}

bool Engine::runWorkerSampleWithCpuBinding() {
    /**
     * Register a worker with its name
     */
    registerWorker<WorkerSampleWithCpuBinding>("WorkerSampleWithCpuBinding");

    /**
     * You can pass a variable to the worker as a shared pointer.
     */
    std::shared_ptr<int> hoge(new int(123));

    /**
     * Run the worker with cpu binding
     */
    SPDLOG_INFO(puManager->puListStr());
    runWorkerWithCpuBinding("WorkerSampleWithCpuBinding", hoge);

    SPDLOG_INFO(puManager->puListStr());
    std::this_thread::sleep_for(std::chrono::milliseconds (200));
    registerWorker<WorkerSampleWithCpuBinding>("WorkerSampleWithCpuBinding2");
    runWorkerWithCpuBinding("WorkerSampleWithCpuBinding2");

    SPDLOG_INFO(puManager->puListStr());

    std::this_thread::sleep_for(std::chrono::milliseconds (200));
    registerWorker<WorkerSampleWithCpuBinding>("WorkerSampleWithCpuBinding3");
    runWorkerWithCpuBinding("WorkerSampleWithCpuBinding3");

    SPDLOG_INFO(puManager->puListStr());

    return true;

}