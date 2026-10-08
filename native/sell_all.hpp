#pragma once
#include <windows.h>
#include <array>
#include <algorithm>
#include <cmath>
#include <cstring>
#include "drop_all.hpp"

namespace wht {
// Local read natives only. Selling uses the synchronized 0x13 engine packet.
// Scan the unit list in bounded chunks; never scan items or guess neutral shops.
class SellAll {
    struct Shop { DropItem object; uintptr_t unit=0; float distance=0; };
    DropSnapshot owner_{};
    std::array<DropItem,6> original_{};
    std::array<bool,6> ordered_{};
    std::array<Shop,32> shops_{};
    unsigned shopCount_=0, scan_=0, slot_=0, shop_=0, passes_=0;
    uint32_t next_=0, deadline_=0;
    uint32_t player_=0;
    bool active_=false, scanning_=false, waiting_=false, confirming_=false;
    static bool Readable(uintptr_t p,size_t n) {
        MEMORY_BASIC_INFORMATION m{};
        return p&&VirtualQuery(reinterpret_cast<void*>(p),&m,sizeof(m))==sizeof(m)&&m.State==MEM_COMMIT&&
            !(m.Protect&(PAGE_GUARD|PAGE_NOACCESS))&&n<=m.RegionSize-(p-reinterpret_cast<uintptr_t>(m.BaseAddress));
    }
    static uintptr_t Ptr(uintptr_t p) {return Readable(p,4)?*reinterpret_cast<uintptr_t*>(p):0;}
    static DropObjectId Id(uintptr_t p) {return {static_cast<uint32_t>(Ptr(p+12)),static_cast<uint32_t>(Ptr(p+16))};}
    static uint32_t Native(uintptr_t g,const char* name,uint32_t h) {
        return reinterpret_cast<uint32_t(__cdecl*)(uint32_t)>(LookupJassNative(g,name))(h);
    }
    static float Real(uintptr_t g,const char* name,uint32_t h) {
        auto bits=Native(g,name,h);float f;memcpy(&f,&bits,4);return f;
    }
    static uintptr_t Object(uintptr_t g,uint32_t h) {
        return reinterpret_cast<uintptr_t(__fastcall*)(uint32_t,int)>(g+ActiveGameOffsets().actionPacketPlayer)(h,0);
    }
    static uint32_t Handle(uintptr_t g,uintptr_t context,uintptr_t p) {
        return reinterpret_cast<uint32_t(__thiscall*)(uintptr_t,uintptr_t,int)>(g+ActiveGameOffsets().lookupUnit)(context,p,0);
    }
    static uintptr_t Session(uintptr_t g) {
        const auto tls=reinterpret_cast<uintptr_t(__fastcall*)(unsigned,unsigned)>(g+ActiveGameOffsets().eventContext)(13,0);
        const auto s=Ptr(Ptr(tls+0x10)+8);
        return Readable(s,0x614)&&Ptr(s+0x278)>=3&&!Ptr(s+0x610)?s:0;
    }
    static bool Merchant(uintptr_t g,uintptr_t p) {
        auto ability=reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t,uint32_t,int,int,int,int)>(g+ActiveGameOffsets().unitGetAbility);
        // Require Purchase Item. Share Inventory alone can transfer the item
        // into a recipient's inventory without paying the seller.
        return ability(p,0x41706974,0,1,1,1)!=0;
    }
    static bool Accessible(uintptr_t g,uint32_t shop,uint32_t local) {
        const auto owner=Native(g,"GetOwningPlayer",shop);
        if(Native(g,"GetPlayerId",owner)>=12)return true;
        return reinterpret_cast<uint32_t(__cdecl*)(uint32_t,uint32_t)>(LookupJassNative(g,"IsPlayerAlly"))(owner,local)!=0;
    }
    bool Snapshot(uintptr_t g,uintptr_t context,uintptr_t p,uintptr_t session,DropSnapshot& out) {
        if(!Readable(p,0x20)||!Id(p).Valid())return false;
        const auto h=Handle(g,context,p);
        const auto local=reinterpret_cast<uint32_t(__cdecl*)()>(LookupJassNative(g,"GetLocalPlayer"))();
        if(!h||!Native(g,"GetUnitTypeId",h)||Native(g,"GetOwningPlayer",h)!=local||!(Real(g,"GetWidgetLife",h)>0.405f))return false;
        out={};out.root=Ptr(g+ActiveGameOffsets().root);out.session=session;out.unit=p;out.handle=h;out.id=Id(p);
        out.x=Real(g,"GetUnitX",h);out.y=Real(g,"GetUnitY",h);
        if(!std::isfinite(out.x)||!std::isfinite(out.y))return false;
        auto slot=reinterpret_cast<uint32_t(__cdecl*)(uint32_t,int)>(LookupJassNative(g,"UnitItemInSlot"));
        for(unsigned i=0;i<6;++i){auto item=slot(h,i);if(item&&Native(g,"GetItemTypeId",item)){auto pItem=Object(g,item);if(Readable(pItem,0x20))out.items[i]={item,Id(pItem)};}}
        return true;
    }
    unsigned Held(const DropSnapshot& s) {
        unsigned count=0;
        for(unsigned i=0;i<original_.size();++i){
            auto& item=original_[i];
            if(!item.handle)continue;
            bool held=false;for(const auto& current:s.items)if(current.handle==item.handle&&current.id==item.id)held=true;
            if(!held){if(ordered_[i])++sold;item={};ordered_[i]=false;}else ++count;
        }
        return count;
    }
    void AddShop(const Shop& shop) {
        for(unsigned i=0;i<shopCount_;++i)if(shops_[i].object.id==shop.object.id)return;
        unsigned pos=shopCount_;
        if(pos==shops_.size()){if(shop.distance>=shops_[pos-1].distance)return;--pos;}
        else ++shopCount_;
        while(pos&&shops_[pos-1].distance>shop.distance){shops_[pos]=shops_[pos-1];--pos;}
        shops_[pos]=shop;
    }
    bool Submit(uintptr_t g,const DropSnapshot& seller,const DropItem& item,const Shop& shop) {
        auto target=seller;target.id=shop.object.id;
        auto bytes=DropAction(target,item);
        // The old six-packet burst required queued orders. This serialized
        // worker must interrupt the current order (and a rejected shop order).
        bytes[1]=0;
        struct Buffer {uintptr_t table;const void* data;uint32_t position,reserved,size,capacity;};
        Buffer buffer{g+ActiveGameOffsets().actionPacketVtable,bytes.data(),0,0,39,0xffffffff};
        reinterpret_cast<void(__fastcall*)(Buffer*,DWORD)>(g+ActiveGameOffsets().transferSend)(&buffer,0);
        return true;
    }
public:
    enum class Result {Idle,Working,Complete,NoShop,Remaining,Cancelled,Empty};
    unsigned submitted=0, sold=0, remaining=0;
    bool singleItem=false;
    bool Active()const{return active_;}
    void Reset(){*this=SellAll{};}
    void Cancel(){active_=false;scanning_=waiting_=confirming_=false;}
private:
    Result TickNative(uintptr_t g,uint32_t now,bool start,int selectedSlot) {
        const auto session=Session(g), players=Ptr(g+ActiveGameOffsets().world);
        if(!session||!Readable(players,0x98)){Reset();return Result::Cancelled;}
        const auto index=*reinterpret_cast<uint16_t*>(players+0x28);
        if(index>=16){Cancel();return Result::Cancelled;}
        const auto context=reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t)>(g+ActiveGameOffsets().getUnitManager)(players);
        if(!Readable(context,4)){Cancel();return Result::Cancelled;}
        const auto selected=Ptr(Ptr(Ptr(players+0x58+index*4)+0x34)+0x1e0);
        DropSnapshot snap{};
        if(!active_){
            if(!start)return Result::Idle;
            Snapshot(g,context,selected,session,snap);
            if(!snap.handle)return Result::Cancelled;
            owner_=snap;original_=snap.items;ordered_={};submitted=sold=0;remaining=Held(snap);
            singleItem=selectedSlot>=0;
            if(selectedSlot>=0){for(int i=0;i<6;++i)if(i!=selectedSlot)original_[i]={};remaining=Held(snap);}
            player_=Native(g,"GetOwningPlayer",snap.handle);
            if(!remaining)return Result::Empty;
            active_=scanning_=true;waiting_=confirming_=false;scan_=slot_=shop_=shopCount_=passes_=0;deadline_=now+16000;next_=now;
            return Result::Working;
        }
        if(!Snapshot(g,context,owner_.unit,session,snap)||snap.root!=owner_.root||snap.session!=owner_.session||!(snap.id==owner_.id)){
            Cancel();return Result::Cancelled;
        }
        // The engine packet acts on selection. Never sell another unit's items
        // or change the user's selection to keep this worker running.
        if(selected!=owner_.unit){Cancel();return Result::Cancelled;}
        remaining=Held(snap);
        if(waiting_&&slot_<6&&!original_[slot_].handle){
            // Inventory and gold update independently. Held counts completed
            // orders by original item, including late replies from earlier slots.
            if(!confirming_){confirming_=true;next_=now+25;}
            if(now-next_>=0x80000000u)return Result::Working;
            waiting_=confirming_=false;shop_=0;
        }
        if(!remaining){Cancel();return Result::Complete;}
        if(now-deadline_<0x80000000u){Cancel();return Result::Remaining;}
        if(now-next_>=0x80000000u)return Result::Working;
        if(scanning_){
            const auto manager=Ptr(snap.root+0x3bc),count=Ptr(manager+0x604),list=Ptr(manager+0x608);
            if(count>8192||(count&&!Readable(list,count*4))){Cancel();return Result::Cancelled;}
            const unsigned end=(std::min)(static_cast<unsigned>(count),scan_+64);
            for(;scan_<end;++scan_){
                const auto p=Ptr(list+scan_*4);if(p==owner_.unit||!Readable(p,0x20))continue;
                const auto h=Handle(g,context,p);if(!h||!Native(g,"GetUnitTypeId",h))continue;
                const float x=Real(g,"GetUnitX",h),y=Real(g,"GetUnitY",h),dx=x-snap.x,dy=y-snap.y,d=dx*dx+dy*dy;
                // Cheap distance check precedes ability lookups.
                if(!std::isfinite(d)||d>1200*1200||!(Real(g,"GetWidgetLife",h)>0.405f)||!Accessible(g,h,player_)||!Merchant(g,p))continue;
                AddShop({{h,Id(p)},p,d});
            }
            if(scan_>=count){scanning_=false;if(!shopCount_){Cancel();return Result::NoShop;}}
            next_=now+8;return Result::Working;
        }
        while(slot_<6&&!original_[slot_].handle){++slot_;shop_=0;waiting_=false;}
        if(slot_==6){
            // The first order may only interrupt combat or begin approaching
            // the shop. Retry original items still held, with a bounded deadline.
            if(passes_<2){++passes_;slot_=shop_=0;waiting_=false;next_=now+100;return Result::Working;}
            Cancel();return Result::Remaining;
        }
        // The drop-on-target order can put a non-pawnable item on the ground.
        // Keep it in the inventory instead of mistaking that for a sale.
        if(!Native(g,"IsItemPawnable",original_[slot_].handle)){
            ++slot_;shop_=0;waiting_=false;next_=now+8;return Result::Working;
        }
        if(waiting_){waiting_=false;++shop_;}
        while(shop_<shopCount_){
            const auto& shop=shops_[shop_];
            const float dx=Real(g,"GetUnitX",shop.object.handle)-snap.x,dy=Real(g,"GetUnitY",shop.object.handle)-snap.y;
            if(!(Id(shop.unit)==shop.object.id)||!Native(g,"GetUnitTypeId",shop.object.handle)||
                !(Real(g,"GetWidgetLife",shop.object.handle)>0.405f)||!Accessible(g,shop.object.handle,player_)||!Merchant(g,shop.unit)||!std::isfinite(dx)||!std::isfinite(dy)||dx*dx+dy*dy>1200*1200){++shop_;continue;}
            Submit(g,snap,original_[slot_],shop);ordered_[slot_]=true;++submitted;waiting_=true;
            next_=now+(dx*dx+dy*dy>600*600?4000:350);return Result::Working;
        }
        ++slot_;shop_=0;next_=now+8;return Result::Working;
    }
public:
    Result Tick(uintptr_t g,uint32_t now,bool start=false,int selectedSlot=-1) {
        if(selectedSlot < -1 || selectedSlot>=6)return Result::Cancelled;
        if(!g){Reset();return Result::Cancelled;}
        if(!active_&&!start)return Result::Idle;
        if(active_&&start)return Result::Working;
        __try{return TickNative(g,now,start,selectedSlot);}
        __except(EXCEPTION_EXECUTE_HANDLER){Cancel();return Result::Cancelled;}
    }
};
}
