//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_ENGINESAMPLE_H
#define ISLAY_ENGINESAMPLE_H

#include <islay/Engine.h>

class EngineSample : public EngineBase{
public:
    EngineSample(AppMsgPtr _appMsg): EngineBase(std::move(_appMsg)){};
    ~EngineSample(){
        reset();
    }
    bool run() override;
    bool reset() override {
        for(auto& [name, worker]: workers) worker.reset();
        return true;
    };

    bool runWorkerSample();
    bool runWorkerSampleWithAppMsg();
};


#endif //ISLAY_ENGINESAMPLE_H
