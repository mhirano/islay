//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_WORKERTEST_H
#define ISLAY_WORKERTEST_H

#include "Engine.h"
#include "AppMsg.h"

/**
 * @brief Example class of worker without application messenger
 */
class Test : public WorkerBase {
public:
    explicit Test():WorkerBase(){};
    bool run(const std::shared_ptr<void> data);
};

/**
 * @brief Example class of worker with application messenger
 */
class TestWithAppMsg : public WorkerBase {
public:
    explicit TestWithAppMsg (AppMsgPtr appMsg):WorkerBase(appMsg){};
    bool run(const std::shared_ptr<void> data);
};


#endif //ISLAY_WORKERTEST_H
