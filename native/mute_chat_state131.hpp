#pragma once
#include <atomic>
#include <cstdint>
#include <string>
#include <sstream>
namespace wht {
inline bool ParseMuteSlot131(const std::wstring& name,const std::wstring& args,unsigned& slot){
 slot=0;if((name!=L"#mute"&&name!=L"#unmute")||args.find(L'\0')!=std::wstring::npos||args.find_first_of(L"\r\n")!=std::wstring::npos)return false;
 std::wistringstream in(args);std::wstring value,extra;if(!(in>>value)||in>>extra||value.size()>2||value.find_first_not_of(L"0123456789")!=std::wstring::npos)return false;
 unsigned n=0;for(auto c:value)n=n*10+unsigned(c-L'0');if(!n||n>24)return false;slot=n;return true;
}
struct MuteChatState131 {
 static constexpr unsigned active=1u<<29,localMask=31u<<24;
 std::atomic<unsigned> packed{0};uintptr_t world=0;
 void Reset(){world=0;packed.store(0,std::memory_order_release);}
 void Publish(bool allowed,uintptr_t current,unsigned local){
  if(!allowed||!current||local>=24){Reset();return;}
  const unsigned context=active|(local<<24);unsigned value=packed.load(std::memory_order_acquire);
  if(world!=current||(value&(active|localMask))!=context){world=current;packed.store(context,std::memory_order_release);}
 }
 bool Ready()const{return (packed.load(std::memory_order_acquire)&active)!=0;}
 bool Set(unsigned slot,bool enabled){
  if(!slot||slot>24)return false;const unsigned sender=slot-1,bit=1u<<sender;unsigned value=packed.load(std::memory_order_acquire);
  for(;;){if(!(value&active)||((value&localMask)>>24)==sender)return false;const unsigned next=enabled?value|bit:value&~bit;if(packed.compare_exchange_weak(value,next,std::memory_order_acq_rel))return true;}
 }
 bool Hidden(unsigned sender,int channel)const{
  if(sender>=24||channel<0||channel>2)return false;const unsigned value=packed.load(std::memory_order_acquire);
  return (value&active)&&((value&localMask)>>24)!=sender&&(value&(1u<<sender));
 }
};
inline MuteChatState131 mutedChat131;
template<class Forward> bool DispatchMuteDisplay131(bool owner,uintptr_t object,unsigned sender,const char* text,int channel,float duration,Forward forward){
 if(owner&&mutedChat131.Hidden(sender,channel))return false;forward(object,sender,text,channel,duration);return true;
}
}
