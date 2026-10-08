#pragma once
#include <mutex>
namespace wht {
// The native scheduler waits on Windows timers. Without a fine timer period,
// a 16 ms frame interval can round up to about 30 ms on this system.
struct FpsTimerState {
    std::mutex mutex;
    bool active=false;
    template<class Begin,class End>
    bool Apply(bool wanted,Begin begin,End end){
        std::lock_guard<std::mutex> lock(mutex);
        if(wanted==active)return true;
        if(wanted){if(begin(1)!=0)return false;active=true;}
        else {if(end(1)!=0)return false;active=false;}
        return true;
    }
};
}
