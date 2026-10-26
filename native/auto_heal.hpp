#pragma once
#include <array>
#include <vector>
#include <string>
#include <cmath>
#include <cstdint>
#include <algorithm>
#include <sstream>
namespace wht {
inline uint32_t HealItemCode(const std::wstring& text){
 if(text.size()!=4)return 0;uint32_t code=0;
 for(auto ch:text){if(ch<33||ch>126)return 0;code=(code<<8)|uint32_t(ch);}return code;
}
inline std::vector<uint32_t> HealItemCodes(const std::wstring& text){
 std::vector<uint32_t> result;size_t begin=0;
 while(begin<text.size()&&result.size()<32){auto end=text.find(L',',begin);auto part=text.substr(begin,end==std::wstring::npos?end:end-begin);
  const auto code=HealItemCode(part);if(code&&std::find(result.begin(),result.end(),code)==result.end())result.push_back(code);
  if(end==std::wstring::npos)break;begin=end+1;
 }return result;
}
inline std::wstring HealItemList(const std::vector<uint32_t>& items,uint32_t extra=0){
 std::wstring result;auto append=[&](uint32_t code){if(!code)return;if(!result.empty())result+=L',';for(int i=3;i>=0;--i)result+=wchar_t((code>>(i*8))&255);};
 for(auto code:items)append(code);if(extra&&std::find(items.begin(),items.end(),extra)==items.end()&&items.size()<32)append(extra);return result;
}
struct AutoPotionSample {uint64_t identity=0;float value=0,maximum=0;std::array<uint32_t,6> items{};bool alive=true;};
using AutoHealSample=AutoPotionSample;
using AutoManaSample=AutoPotionSample;
struct AutoPotionState {
 uint64_t due=0,retry=0;unsigned cursor=0;
 template<class Read,class Use> bool Tick(bool enabled,bool allowed,float percent,const std::vector<uint32_t>& items,uint64_t now,Read read,Use use,bool mana=false){
  if(!enabled||now<due||now<retry)return false;due=now+200;
  if(!allowed)return false;
  AutoPotionSample sample;if(!read(sample)||!sample.identity||!sample.alive||!std::isfinite(sample.value)||!std::isfinite(sample.maximum)||sample.maximum<=0||(mana?sample.value<0:sample.value<=.405f)||!std::isfinite(percent)||percent<1||percent>99||double(sample.value)*100>=double(sample.maximum)*percent)return false;
  for(unsigned n=0;n<6;++n){const auto slot=(cursor+n)%6,code=sample.items[slot];
   if(!code||std::find(items.begin(),items.end(),code)==items.end())continue;
   // A failed/cooling-down item must not be clicked every frame, nor consume
   // several potions before the synchronized game state reflects the heal.
   cursor=(slot+1)%6;retry=now+1500;
   return use(sample.identity,slot,code);
  }return false;
 }
};
using AutoHealState=AutoPotionState;
using AutoManaState=AutoPotionState;
inline bool IsAutoManaCommand(const std::wstring& name){return name==L"#automana"||name==L"#healmp"||name==L"#manaitem";}
inline bool IsAutoHealCommand(const std::wstring& name){return name==L"#autoheal"||name==L"#healhp"||name==L"#healitem";}
template<class Config,class Slot> bool AutoHealSetting(const std::wstring& name,std::wistream& input,const Config& c,Slot readSlot,const wchar_t*& key,std::wstring& value){
 std::wstring arg,extra;
 if(name==L"#autoheal"){
  key=L"Enabled";value=c.autoHealEnabled?L"0":L"1";
  if(input>>arg){if((arg!=L"0"&&arg!=L"1")||input>>extra)return false;value=arg;}return true;
 }
 if(name==L"#healhp"){
  double percent=0;if(!(input>>percent)||input>>extra||!std::isfinite(percent)||percent<1||percent>99||percent!=std::floor(percent))return false;
  key=L"Percent";value=std::to_wstring(unsigned(percent));return true;
 }
 if(name==L"#healitem"){
  unsigned slot=0;if(!(input>>slot)||slot<1||slot>6||input>>extra)return false;
  const auto code=readSlot(slot-1);if(!code)return false;
  if(c.autoHealItems.size()>=32&&std::find(c.autoHealItems.begin(),c.autoHealItems.end(),code)==c.autoHealItems.end())return false;
  key=L"ItemTypes";value=HealItemList(c.autoHealItems,code);return true;
 }return false;
}

template<class Config,class Slot> bool AutoManaSetting(const std::wstring& name,std::wistream& input,const Config& c,Slot readSlot,const wchar_t*& key,std::wstring& value){
 struct View {bool autoHealEnabled;const std::vector<uint32_t>& autoHealItems;};
 const View view{c.autoManaEnabled,c.autoManaItems};
 if(!IsAutoManaCommand(name))return false;
 return AutoHealSetting(name==L"#automana"?L"#autoheal":name==L"#healmp"?L"#healhp":L"#healitem",input,view,readSlot,key,value);
}

}
