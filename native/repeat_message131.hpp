#pragma once
#include "quick_chat131.hpp"
#include "repeat_message129.hpp"
namespace wht {
// Same count and 250ms cadence as 1.29; current native chat owns delivery.
struct RepeatMessage131 {
 RepeatMessage129 queue;
 void Reset(){queue.Reset();}
 template<class Notify> bool Command(const std::wstring& args,Notify notify){
  unsigned count=0;std::wstring text;bool stop=false;
  if(args.find_first_of(L"\r\n")!=std::wstring::npos||args.find(L'\0')!=std::wstring::npos||!ParseRepeatMessage129(args,count,text,stop)){
   notify("Usage: #repeat <1..10000> <text>; #repeat stop");return false;
  }
  if(stop){Reset();notify("Repeat stopped");return true;}
  const auto value=Utf8(text);
  if(!ValidQuickChat131(value)||!queue.Set(count,value)){
   notify("Repeat text: 1..128 UTF-8 bytes; no hash commands or line breaks");return false;
  }
  notify("Repeat queued: "+std::to_string(count)+" messages");return true;
 }
 bool Command(const std::wstring& args){return Command(args,[](const std::string&){});}
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
