//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_ENGINE_H
#define ISLAY_ENGINE_H

#include <islay/EngineBase.h>

class Engine : public EngineBase{
public:
    Engine(AppMsgPtr _appMsg): EngineBase(std::move(_appMsg)){};
    ~Engine(){
        reset();
    }
    bool run() override;
    bool reset() override {
        for(auto& [name, worker]: workers) worker.reset();
    };

    bool runWorkerSample();
    bool runWorkerSampleWithAppMsg();
};


#endif //ISLAY_ENGINE_H
