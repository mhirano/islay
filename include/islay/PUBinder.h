//
// Created by Hirano Masahiro <masahiro.dll@gmail.com>
//

#ifndef ISLAY_PUBINDER_H
#define ISLAY_PUBINDER_H

#include <thread>
#include <hwloc.h>

#if WIN32
#include <windows.h>
#else
#endif

class WorkerBase;
template<typename T>
class WorkerManager;

inline unsigned int id_to_uint(std::thread::id id){
    std::ostringstream os;
    os.clear(); os.str("");
    os << id;
    return std::stoi(os.str());
}

struct PUManager{
    PUManager(){
        hwloc_topology_init(&topology);
        hwloc_topology_load(topology);
        int topodepth = hwloc_topology_get_depth(topology);
        pu_num = hwloc_get_nbobjs_by_depth(topology, topodepth-1); // Get PU lists
        SPDLOG_INFO("Number of processing units: {}", pu_num);
        pu_list.resize(pu_num); // -1 means unbinded
        std::fill(pu_list.begin(), pu_list.end(), "");
    }

    ~PUManager(){
        hwloc_topology_destroy(topology);
    }

    int bindThread(std::string workerName, std::thread::native_handle_type thread, std::thread::id id){
        // Pick a vacant PU to bind the thread
        const auto firstUnbindedPu = std::find(pu_list.begin(), pu_list.end(), "");
        if(firstUnbindedPu == pu_list.end()){
            return -1;
        } else {
            // Get the index of the vacant PU
            const unsigned int ind = std::distance(pu_list.begin(), firstUnbindedPu);
            // Get the vacant pu object
            hwloc_obj_t pu = hwloc_get_obj_by_type(topology, hwloc_obj_type_t::HWLOC_OBJ_PU, ind);
            // Bind the thread (identified by handle) with the vacant PU
            hwloc_set_thread_cpubind(topology, thread, pu->cpuset, HWLOC_CPUBIND_THREAD);
            pu_list[pu->logical_index] = workerName;
            return ind;
        }
    }

    hwloc_topology_t topology;
    unsigned int pu_num;
    std::vector<std::string> pu_list; /// Consider having a pointer to workermanager
};

#endif //ISLAY_PUBINDER_H
