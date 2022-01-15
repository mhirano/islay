//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_ENGINESAMPLE_H
#define ISLAY_ENGINESAMPLE_H

#include <islay/Engine.h>

class EngineSample : public Engine{
public:
    EngineSample(AppMsgPtr _appMsg): Engine(std::move(_appMsg)){};
    ~EngineSample(){
        reset();
    }
    bool run() override;
    bool reset() override {
        for(auto& [name, worker]: workers) worker.reset();
    };
};


#endif //ISLAY_ENGINESAMPLE_H
