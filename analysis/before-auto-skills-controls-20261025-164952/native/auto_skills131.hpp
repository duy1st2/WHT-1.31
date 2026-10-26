#pragma once
#include "self_cast131.hpp"
#include "all_skills131.hpp"
#include <unordered_map>
#include <sstream>
inline bool AutoSkillClearIndex131(const std::wstring& args,int& index){
 if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring value,extra;if(!(in>>value)||(in>>extra))return false;
 if(value==L"all"){index=-1;return true;}if(value.size()!=1||value[0]<L'1'||value[0]>L'9')return false;index=int(value[0]-L'1');return true;
}
namespace wht {
inline int AutoSkillNumber131(unsigned key){if(key>='1'&&key<='9')return int(key-'1');if(key>=VK_NUMPAD1&&key<=VK_NUMPAD9)return int(key-VK_NUMPAD1);return -1;}
inline bool AutoSkillOrder131(const SkillButton131& b){if(!b.order||b.ability==AllSkills131::heroAbility)return false;switch(b.order){case 0xd0003:case 0xd0004:case 0xd000f:case 0xd0012:case 0xd0016:case 0xd0019:case 0xd0020:case 0xd0007:return false;default:return true;}}
inline bool SameAutoSkill131(const SkillButton131& a,const SkillButton131& b){return a.order==b.order&&a.ability==b.ability;}
struct AutoSkillScan131 {unsigned cursor=0;SelfCastSource131 owner,best;double distance=INFINITY;};
enum class AutoScanResult131 {Working,Done,Failed};
struct AutoSkills131 {
 struct Sequence {SelfCastSource131 owner;std::array<SkillButton131,9> bindings{};unsigned cursor=0;};
 std::unordered_map<uintptr_t,Sequence> sequences;bool running=false,scanning=false;uint64_t due=0;AutoSkillScan131 scan;SkillButton131 requested{};unsigned slot=12;
 void CancelScan(){scanning=false;scan={};requested={};slot=12;}
 void Reset(){sequences.clear();running=false;due=0;CancelScan();}
 bool Bind(const SelfCastSource131& owner,int index,const SkillButton131& data){
  if(index<0||index>=9||!owner.ui||!owner.unit||!owner.session||!owner.id.Valid()||!AutoSkillOrder131(data))return false;
  auto found=sequences.find(owner.unit);if(found==sequences.end()){if(sequences.size()>=256)return false;found=sequences.emplace(owner.unit,Sequence{}).first;}
  auto& sequence=found->second;if(!SameSelfCastSource131(sequence.owner,owner))sequence={};sequence.owner=owner;sequence.bindings[index]=data;sequence.cursor=0;CancelScan();due=0;return true;
 }
 Sequence* Current(const SelfCastSource131& source){auto found=sequences.find(source.unit);if(found==sequences.end()||!SameSelfCastSource131(found->second.owner,source))return nullptr;return &found->second;}
 bool Toggle(const SelfCastSource131& source,bool next){
  if(next){auto* sequence=Current(source);if(!sequence)return false;bool any=false;for(const auto& b:sequence->bindings)any|=AutoSkillOrder131(b);if(!any)return false;}
  running=next;due=0;CancelScan();return true;
 }
 bool Clear(const SelfCastSource131& source,int index){
  if(index < -1 || index >= 9)return false;auto* sequence=Current(source);if(!sequence)return false;
  if(index<0)sequence->bindings={};else sequence->bindings[index]={};
  sequence->cursor=0;CancelScan();due=0;
  bool any=false;for(const auto& b:sequence->bindings)any|=AutoSkillOrder131(b);
  if(!any){sequences.erase(source.unit);running=false;}return true;
 }
 std::string Summary(const SelfCastSource131& source){
  auto* sequence=Current(source);std::string text="Auto Skills: ";text+=running?"ON; slots:":"OFF; slots:";
  bool any=false;if(sequence)for(unsigned i=0;i<9;++i)if(AutoSkillOrder131(sequence->bindings[i])){text+=" "+std::to_string(i+1);any=true;}
  if(!any)text+=" empty";return text;
 }
 template<class Api> void Tick(uint64_t now,bool enabled,bool ready,float delay,float range,Api& api){
  if(!enabled){running=false;CancelScan();return;}if(!running||!ready||(!scanning&&now<due))return;
  if(!std::isfinite(delay)||!std::isfinite(range)||delay<.05f||delay>10||range<128||range>4096)return;
  const auto milliseconds=uint64_t(std::clamp(delay,.05f,10.f)*1000.f);
  SelfCastSource131 source;if(!api.ReadSource(source)){CancelScan();due=now+milliseconds;return;}
  if(scanning&&!SameSelfCastSource131(source,scan.owner)){CancelScan();due=now+milliseconds;return;}
  if(scanning){const double dx=double(source.x)-scan.owner.x,dy=double(source.y)-scan.owner.y;if(dx*dx+dy*dy>48.*48.){CancelScan();due=now+milliseconds;return;}}
  auto* sequence=Current(source);if(!sequence){CancelScan();due=now+milliseconds;return;}
  if(!scanning){
   due=now+milliseconds;unsigned found=9;for(unsigned i=0;i<9;++i){const unsigned index=(sequence->cursor+i)%9;if(AutoSkillOrder131(sequence->bindings[index])){found=index;break;}}if(found==9)return;
   requested=sequence->bindings[found];sequence->cursor=(found+1)%9;slot=12;
   for(unsigned i=0;i<12;++i)if(SameAutoSkill131(api.Data(i),requested)){slot=i;break;}if(slot==12)return;
   scan={};scan.owner=source;scan.distance=double(range)*range;scanning=true;
  }
  if(!SameAutoSkill131(api.Data(slot),requested)){CancelScan();return;}
  const auto result=api.Scan(source,range,scan);if(result==AutoScanResult131::Working)return;
  if(result==AutoScanResult131::Done&&scan.best.unit)api.Activate(source,scan.best,slot,requested,range);
  CancelScan();due=now+milliseconds;
 }
};
// The first click may execute an instant ability. Never attempt a second order
// path after an uncertain native result; cancel only the new owned target mode.
template<class Source,class Destination,class Target,class Click,class Order,class Finish,class Cancel>
bool ActivateAutoSkill131(Source readSource,Destination readDestination,Target target,Click click,Order order,Finish finish,Cancel cancel){
 if(target())return false;SelfCastSource131 before,after,destination,current;
 if(!readSource(before)||!readDestination(destination)||before.ui!=destination.ui||before.session!=destination.session||!click())return false;
 const auto mode=target();if(!mode)return true;
 if(!readSource(after)||!SameSelfCastSource131(before,after)||!readDestination(current)||!SameSelfCastSource131(destination,current)){if(target()==mode)cancel(mode);return false;}
 const auto saved=order(mode);if(!saved){if(target()==mode)cancel(mode);return false;}return finish(current,mode,saved);
}
}
