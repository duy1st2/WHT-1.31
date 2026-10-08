#pragma once
#include <cstdint>

namespace wht {
// One action at most per tick. Never carry a due action across a match, config
// change, disable or suspended input state, and never replay missed intervals.
struct PeriodicAction {
    uintptr_t owner=0;
    unsigned period=0;
    uint64_t due=0;
    bool armed=false;
    bool Poll(uintptr_t currentOwner,bool enabled,unsigned seconds,uint64_t now){
        if(!currentOwner||!enabled||seconds<1||seconds>17){*this={};return false;}
        if(!armed||owner!=currentOwner||period!=seconds){
            owner=currentOwner;period=seconds;due=now+seconds*1000ULL;armed=true;return false;
        }
        if(now<due)return false;
        due=now+seconds*1000ULL;return true;
    }
};
}
