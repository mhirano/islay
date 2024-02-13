//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#include "Engine.h"
#include "WorkerSample.h"

bool Engine::run() {
    return true;
}

bool Engine::runWorkerSample() {
    /*
     * Register a worker with its name
     */
    registerWorker<WorkerSample>("WorkerSample");

    /*
     * You can pass a variable to the worker as a shared pointer.
     */
    std::shared_ptr<int> hoge(new int(123));

    /*
     * Run the worker
     */
    runWorker("WorkerSample", hoge);

    return true;
}

bool Engine::runWorkerSampleWithAppMsg() {
    /*
     * Register a worker with its name
     */
    registerWorkerWithAppMsg<WorkerSampleWithAppMsg>("WorkerSampleWithAppMsg");

    /*
     * You can pass a variable to the worker as a shared pointer.
     */
    std::shared_ptr<int> hoge(new int(123));

    /*
     * Run the worker
     */
    runWorker("WorkerSampleWithAppMsg", hoge);

    return true;
}