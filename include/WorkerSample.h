//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_WORKERSAMPLE_H
#define ISLAY_WORKERSAMPLE_H

#include <islay/Worker.h>

/** \brief Sample class of worker with application messenger
 *
 */
class WorkerSample : public WorkerBase {
public:
    explicit WorkerSample (AppMsgPtr appMsg): WorkerBase(appMsg){};
    bool run(const std::shared_ptr<void> data);
};

/** A\brief Sample class of worker with cpu binding
 *
 */
class WorkerSampleWithCpuBinding : public WorkerBase {
public:
    explicit WorkerSampleWithCpuBinding (AppMsgPtr appMsg): WorkerBase(appMsg){};
    bool run(const std::shared_ptr<void> data);

};

#endif //ISLAY_WORKERSAMPLE_H
