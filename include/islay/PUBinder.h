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

inline std::string id_to_str(std::thread::id id){
    std::ostringstream os;
    os.clear(); os.str("");
    os << id;
    return os.str();
}

class PUBinder{
    hwloc_topology_t topology;
    unsigned int pu_num;
    std::map<unsigned int, std::string> puMap;

public:
    PUBinder(){
        hwloc_topology_init(&topology);
        hwloc_topology_load(topology);
        int topodepth = hwloc_topology_get_depth(topology);
        pu_num = hwloc_get_nbobjs_by_depth(topology, topodepth-1); // Get PU lists
        SPDLOG_INFO("Number of processing units: {}", pu_num);
        for(unsigned int i=0;i<pu_num;i++){
            puMap[i]="";
        }
    }

    ~PUBinder(){
        hwloc_topology_destroy(topology);
    }

    int bindThread(std::string threadName, std::thread::native_handle_type thread, std::thread::id id){
        // Pick a vacant PU to bind the thread
        int firstUnbindedPuLogicalInd;
        for(const auto& [k,v]: puMap){
            if(v=="") {
                firstUnbindedPuLogicalInd = k;
                break;
            }
            firstUnbindedPuLogicalInd = -1;
        }

        if(firstUnbindedPuLogicalInd == -1)
            return firstUnbindedPuLogicalInd;

        // Get the vacant pu object
        hwloc_obj_t pu = hwloc_get_obj_by_type(topology, hwloc_obj_type_t::HWLOC_OBJ_PU, firstUnbindedPuLogicalInd);
        // Bind the thread (identified by handle) with the vacant PU
        hwloc_set_thread_cpubind(topology, thread, pu->cpuset, HWLOC_CPUBIND_THREAD);
        assert(pu->logical_index == firstUnbindedPuLogicalInd && "PU and thread was not binded correctly.");
        puMap[pu->logical_index] = threadName;
        return firstUnbindedPuLogicalInd;
    }

    bool unbind(std::string threadName){
        for(auto&[k,v]: puMap){
            if(v == threadName) {
                v="";
                /// TODO: check if binded thread is already joined.
                return true;
            }
        }
        return false;
    }

    int getPuIfBinded(std::string threadName){
        for(const auto& [k,v]:puMap){
            if(v == threadName) return k;
        }
        return -1;
    }

    /// Only for debugging
    std::string puListStr(){
        std::ostringstream os;
        os.str(""); os.clear();
        for(const auto& [k,v]: puMap){
            if(v==""){
                os << k << ":" << " " << ", ";
            } else{
                os << k << ":" << v << ", ";
            }
        }
        return os.str();
    }
};

#endif //ISLAY_PUBINDER_H
