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
    registerWorker<WorkerSampleWithCpuBinding>("WorkerSampleWithCpuBinding_0");

    /**
     * You can pass a variable to the worker as a shared pointer.
     */
    std::shared_ptr<int> hoge(new int(123));

    /**
     * Run the worker with cpu binding
     */
    SPDLOG_INFO(puBinder->puListStr());

    runWorkerWithCpuBinding("WorkerSampleWithCpuBinding_0", hoge);
    std::this_thread::sleep_for(std::chrono::milliseconds (10));
    SPDLOG_INFO(puBinder->puListStr());

    registerWorker<WorkerSampleWithCpuBinding>("WorkerSampleWithCpuBinding_1");
    runWorkerWithCpuBinding("WorkerSampleWithCpuBinding_1");
    std::this_thread::sleep_for(std::chrono::milliseconds (10));
    SPDLOG_INFO(puBinder->puListStr());

    registerWorker<WorkerSampleWithCpuBinding>("WorkerSampleWithCpuBinding_2");
    runWorkerWithCpuBinding("WorkerSampleWithCpuBinding_2");
    std::this_thread::sleep_for(std::chrono::milliseconds (10));
    SPDLOG_INFO(puBinder->puListStr());

    return true;

}