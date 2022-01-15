//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_WORKERSAMPLE_H
#define ISLAY_WORKERSAMPLE_H

#include <islay/Worker.h>

/**
 * @brief Sample class of worker without application messenger
 */
class WorkerSample : public WorkerBase {
public:
    explicit WorkerSample(): WorkerBase(){};
    bool run(const std::shared_ptr<void> data);
};

/**
 * @brief Sample class of worker with application messenger
 */
class WorkerSampleWithAppMsg : public WorkerBase {
public:
    explicit WorkerSampleWithAppMsg (AppMsgPtr appMsg): WorkerBase(appMsg){};
    bool run(const std::shared_ptr<void> data);
};


#endif //ISLAY_WORKERSAMPLE_H
