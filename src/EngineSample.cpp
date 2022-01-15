//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#include "EngineSample.h"
#include "WorkerSample.h"

bool EngineSample::run() {
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
