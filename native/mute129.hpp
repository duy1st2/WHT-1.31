#pragma once
#include <atomic>
namespace wht {
struct Mute129 {
 std::atomic<unsigned> mask{0};
 void Reset(){mask.store(0);}
 bool Set(unsigned slot,unsigned local,bool enabled){
  if(slot<1||slot>24||local>=24||slot-1==local)return false;
  const unsigned bit=1u<<(slot-1);
  if(enabled)mask.fetch_or(bit);else mask.fetch_and(~bit);
  return true;
 }
 bool Hidden(unsigned sender,int channel)const{
  return sender<24&&channel>=0&&channel<=2&&(mask.load()&(1u<<sender));
 }
};
}
