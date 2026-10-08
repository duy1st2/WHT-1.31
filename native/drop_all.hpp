#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <cmath>
namespace wht {
struct DropObjectId {
    uint32_t low=0,high=0;
    bool operator==(const DropObjectId& b)const{return low==b.low&&high==b.high;}
    bool Valid()const{return (low||high)&&!(low==0xffffffffu&&high==0xffffffffu);}
};
struct DropItem {uint32_t handle=0;DropObjectId id;};
struct DropSnapshot {
    uintptr_t root=0,session=0,unit=0;uint32_t handle=0;DropObjectId id;
    float x=0,y=0;std::array<DropItem,6> items{};
};
// ETS 100751E0: action 13, queued flag 1, order D0021, two object identities.
inline std::array<uint8_t,39> DropAction(const DropSnapshot& unit,const DropItem& item){
    std::array<uint8_t,39> bytes{};bytes[0]=0x13;bytes[1]=1;
    const uint32_t order=0xd0021,none=0xffffffffu;
    memcpy(bytes.data()+3,&order,4);memcpy(bytes.data()+7,&none,4);memcpy(bytes.data()+11,&none,4);
    memcpy(bytes.data()+15,&unit.x,4);memcpy(bytes.data()+19,&unit.y,4);
    memcpy(bytes.data()+23,&unit.id.low,4);memcpy(bytes.data()+27,&unit.id.high,4);
    memcpy(bytes.data()+31,&item.id.low,4);memcpy(bytes.data()+35,&item.id.high,4);return bytes;
}
enum class DropResult {Idle,Waiting,Submitted,Complete,Cancelled,Remaining};
struct DropAllState {
    bool active=false,settling=false;DropSnapshot owner;
    std::array<DropItem,6> original{};std::array<unsigned,6> attempts{};
    uint64_t next=0,deadline=0;unsigned cursor=0,remaining=0;
    void Reset(){*this={};}
    bool Start(const DropSnapshot& snap,uint64_t now){
        if(active||!snap.root||!snap.session||!snap.unit||!snap.handle||!snap.id.Valid()||!std::isfinite(snap.x)||!std::isfinite(snap.y))return false;
        unsigned count=0;for(const auto& item:snap.items)if(item.handle&&item.id.Valid())++count;
        if(!count)return false;
        Reset();owner=snap;original=snap.items;remaining=count;active=true;next=now;deadline=now+12000;return true;
    }
    template<class Send> DropResult Step(const DropSnapshot* snap,uint64_t now,Send send){
        if(!active)return DropResult::Idle;
        if(!snap||snap->root!=owner.root||snap->session!=owner.session||snap->unit!=owner.unit||snap->handle!=owner.handle||
           !(snap->id==owner.id)||!std::isfinite(snap->x)||!std::isfinite(snap->y)){Reset();return DropResult::Cancelled;}
        remaining=0;
        for(auto& item:original){
            if(!item.handle)continue;
            bool held=false;for(const auto& current:snap->items)if(current.handle==item.handle&&current.id==item.id)held=true;
            if(!held)item={};else ++remaining;
        }
        if(!remaining){Reset();return DropResult::Complete;}
        if(now>=deadline){active=false;return DropResult::Remaining;}
        if(now<next)return DropResult::Waiting;
        // One queued order per game-thread tick, never six immediate local mutations.
        for(;cursor<6;++cursor){
            const unsigned slot=cursor;const auto item=original[slot];
            if(!item.handle||attempts[slot]>=3)continue;
            ++cursor;
            if(!send(*snap,item)){Reset();return DropResult::Cancelled;}
            ++attempts[slot];next=now+100;return DropResult::Submitted;
        }
        bool retry=false;for(unsigned i=0;i<6;++i)if(original[i].handle&&attempts[i]<3)retry=true;
        if(!retry){if(!settling){settling=true;next=now+1000;return DropResult::Waiting;}active=false;return DropResult::Remaining;}
        cursor=0;next=now+1000;return DropResult::Waiting;
    }
};
}
