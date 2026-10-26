#pragma once
#include "inventory_info131.hpp"
inline void CheckInventoryInfo131(){
 wht::InventoryInfo131 output;check(wht::InventoryInfoText131(output,true)=="Item Slots: [Empty]","reference empty inventory feedback");
 output.capacity=6;output.codes[0]=0x49747331;output.codes[5]=0x49747332;check(wht::InventoryInfoText131(output,true)=="Item Slots: [0: Its1] [5: Its2] ","reference rawcode endian and zero-based slot numbering");
 check(wht::InventoryInfoText131(output,false).find("[5: Its2]")!=std::string::npos,"Vietnamese feedback preserves exact rawcodes and last slot");
 wht::InventoryInfo131 binary;binary.capacity=1;binary.codes[0]=0x00417fff;check(wht::InventoryInfoText131(binary,true)=="Item Slots: [0: ?A??] ","nonprinting rawcode bytes sanitized for UI");
 wht::InventoryInfoSource131 source{123,{4,5}};unsigned reads=0;bool change=false,fail=false;unsigned capacity=6;
 auto selected=[&](){auto result=source;if(change&&reads)++result.id.high;return result;};
 auto read=[&](uintptr_t unit,wht::InventoryInfo131& info){check(unit==123,"read uses captured selected unit");++reads;info.capacity=capacity;info.codes[5]=0x49303136;return !fail;};
 check(wht::CaptureInventoryInfo131(selected,read,output)&&output.codes[5]==wht::gokuType131,"snapshot preserves occupied final slot");
 reads=0;change=true;output.codes[0]=1234;check(!wht::CaptureInventoryInfo131(selected,read,output)&&output.codes[0]==1234,"changed object identity refuses partial publication");
 reads=0;change=false;capacity=7;check(!wht::CaptureInventoryInfo131(selected,read,output)&&output.codes[0]==1234,"oversized inventory refused without replacing output");
 reads=0;capacity=1;check(!wht::CaptureInventoryInfo131(selected,read,output),"slots beyond capacity cannot leak stale codes");
 reads=0;capacity=6;fail=true;check(!wht::CaptureInventoryInfo131(selected,read,output),"native read refusal cannot publish partial info");
 source={};reads=0;check(!wht::CaptureInventoryInfo131(selected,read,output)&&reads==0,"missing selection avoids native reads");check(!wht::ReadInventoryInfo131(0,12164,[](){return uintptr_t(0);},output),"missing native profile/base refused");
 wht::LocalInfo131 notice;notice.Event("Item Slots",100);check(notice.Visible(true,false,100)&&notice.until==4100,"read-only output expires after four seconds");check(!notice.Visible(true,true,200)&&notice.Visible(true,false,300),"settings menu masks local info without extending deadline");check(!notice.Visible(true,false,4100),"expired info never replays");
 notice.Event("second",5000);check(!notice.Visible(false,false,5001)&&notice.text.empty(),"lost authorization clears local info");notice.Event(std::string(513,'a'),6000);check(!notice.until&&notice.text.empty(),"oversized feedback refused");notice.Reset();check(!notice.frame&&!notice.label&&!notice.until&&!notice.shown,"teardown forgets all local info frame state");
}
