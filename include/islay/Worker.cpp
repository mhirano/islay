//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#include "Worker.h"

bool WorkerBase::requestCpuBind(
        std::string threadName, std::thread::native_handle_type thread, std::thread::id id
) {
    wm.lock()->puBinder.lock()->bindThread(threadName, thread, id);
    return true;
};
