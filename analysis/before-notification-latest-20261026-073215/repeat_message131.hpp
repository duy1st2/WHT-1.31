#pragma once
#include "quick_chat131.hpp"
#include "repeat_message129.hpp"
namespace wht {
// Same count and 250ms cadence as 1.29; current native chat owns delivery.
struct RepeatMessage131 {
 RepeatMessage129 queue;
 void Reset(){queue.Reset();}
 bool Command(const std::wstring& args){
  if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos)return false;
  unsigned count=0;std::wstring text;bool stop=false;
  if(!ParseRepeatMessage129(args,count,text,stop))return false;
  if(stop){Reset();return true;}
  const auto value=Utf8(text);
  return ValidQuickChat131(value)&&queue.Set(count,value);
 }
 template<class Send> bool Tick(bool session,bool ready,uint64_t now,Send send){
  if(!session){Reset();return false;}
  if(!ready||!queue.remaining||now<queue.due)return false;
  // Back off refused editor calls too; no catch-up burst on late updates.
  queue.due=now+250;
  if(!send(queue.text))return false;
  --queue.remaining;if(!queue.remaining)queue.text.clear();return true;
 }
};
}
