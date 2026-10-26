#pragma once
#include "hp_dimensions131.hpp"
#include <fstream>
inline void CheckHpDimensions131(){
 using namespace wht;
 HpDimensionLease131 lease;float current=.04f;unsigned writes=0;bool succeeds=true;
 auto set=[&](float v){++writes;if(!succeeds)return false;current=v;return true;};
 check(lease.Apply(current,.06f,set)&&current==.06f&&lease.original==.04f&&lease.active,"custom width captures native dimension");
 for(unsigned i=0;i<10000;++i)check(lease.Apply(current,.06f,set),"stable HP dimension");check(writes==1,"unchanged native HP dimensions never invoke setters");
 check(lease.Apply(current,.08f,set)&&current==.08f&&lease.original==.04f,"changing override preserves original");
 check(lease.Apply(current,0,set)&&current==.04f&&!lease.active,"zero restores native dimension");
 lease.Apply(current,.06f,set);current=.07f;check(lease.Apply(current,0,set)&&current==.07f,"disable preserves a new map/native dimension");
 lease.Apply(current,.06f,set);current=.09f;lease.Apply(current,.08f,set);check(lease.original==.09f,"new native value becomes next restore baseline");lease.Apply(current,0,set);check(current==.09f,"latest external baseline restored");
 succeeds=false;check(!lease.Apply(current,.06f,set)&&!lease.active&&current==.09f,"failed setter does not publish active override");succeeds=true;lease.Apply(current,.06f,set);succeeds=false;check(!lease.Apply(current,0,set)&&lease.active,"failed restoration remains retryable");succeeds=true;lease.Apply(current,0,set);
 const unsigned before=writes;for(float bad:{NAN,INFINITY,-1.f,101.f})check(!lease.Apply(current,bad,set),"invalid requested dimensions refused");check(!lease.Apply(NAN,.04f,set)&&writes==before,"invalid native dimension never written");
 float value=9;for(const auto* good:{L"0",L".004",L"0.06",L"100",L" 0.08 "})check(HpDimensionValue131(good,value),"reference bounds and fractional units accepted");
 for(const auto* bad:{L"",L"-0.01",L"100.01",L"nan",L"inf",L"1e99",L".06 extra",L".06\n"})check(!HpDimensionValue131(bad,value),"strict dimension parsing");check(!HpDimensionValue131(std::wstring(L"1\0x",3),value),"NUL refused");
 ViewIniTest131 ini;unsigned reloads=0;Config c;auto save=[&](const wchar_t* section,const wchar_t* key,const std::wstring& v){return SaveSetting(ini.file,section,key,v);};auto reload=[&](){c=LoadConfig(ini.file);++reloads;};
 check(ApplyHpDimension131(L"#whp",L".06",save,reload)&&c.barWidthHp==.06f&&c.barHeightHp==0,"width saves canonical Event/whp independently");check(ApplyHpDimension131(L"#hhp",L".004",save,reload)&&c.barHeightHp==.004f&&c.barWidthHp==.06f,"height saves canonical Event/hhp independently");check(ApplyHpDimension131(L"#whp",L"0",save,reload)&&c.barWidthHp==0&&c.barHeightHp==.004f,"restoring width preserves height");
 check(!ApplyHpDimension131(L"#whp",L".07",[](const wchar_t*,const wchar_t*,const std::wstring&){return false;},reload)&&reloads==3,"failed save never reloads");check(!ApplyHpDimension131(L"#wmp",L"1",save,reload)&&reloads==3,"removed mana bar not enabled");
 for(const auto& p:hpDimensionProfiles131){
#ifdef _WIN64
  const auto file=p.build==12164?"../analysis/memory-6688.bin":"../analysis/memory-29928.bin";
#else
  const auto file=p.build==12164?"../analysis/memory-14740.bin":"../analysis/memory-23884.bin";
#endif
  std::ifstream input(file,std::ios::binary);if(!input)continue;std::vector<char> image((std::istreambuf_iterator<char>(input)),{});const auto base=reinterpret_cast<uintptr_t>(image.data());check(HpDimensionCode131(base,p),"captured native width and height setters verify profile field offsets");image[p.width]^=1;check(!HpDimensionCode131(base,p),"altered width setter refused");image[p.width]^=1;image[p.height]^=1;check(!HpDimensionCode131(base,p),"altered height setter refused");
 }
 HpDimensions131 tracking;tracking.entries[10]={};tracking.entries[20]={};std::unordered_set<uintptr_t> live{20};tracking.Prune(live);check(tracking.entries.size()==1&&tracking.entries.count(20),"destroyed bars pruned using existing frame snapshot");tracking.Reset();check(tracking.entries.empty()&&!tracking.checkedBase,"UI reset forgets pooled frames");
}
