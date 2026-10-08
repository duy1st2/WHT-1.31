#pragma once
#include "state_values129.hpp"
#include <unordered_set>
namespace wht {
using BarVisibility129=void(__thiscall*)(uintptr_t);
inline bool InvulnerableHp129(uintptr_t base,uintptr_t hp,bool& invulnerable){
 __try{
  if(!hp||*reinterpret_cast<uintptr_t*>(hp)!=base+0xa95104||*reinterpret_cast<unsigned*>(hp+0x13c)!=5)return false;
  const auto unit=*reinterpret_cast<uintptr_t*>(hp+0x158);
  invulnerable=unit&&*reinterpret_cast<uintptr_t*>(unit)==base+0xa9c504&&(*reinterpret_cast<unsigned*>(unit+0x20)&8)!=0;return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
struct InvulnerableBar129 {
 bool active=false;
 std::unordered_set<uintptr_t> suppressed;
 void Forget(uintptr_t hp){suppressed.erase(hp);}
 template<class Show,class Hide> bool Update(uintptr_t base,uintptr_t hp,Show show,Hide hide){
  bool invulnerable=false;if(!InvulnerableHp129(base,hp,invulnerable))return false;
  if(active&&invulnerable){if(suppressed.size()>=4096&&!suppressed.count(hp))return false;suppressed.insert(hp);hide(hp);return true;}
  if(suppressed.erase(hp))show(hp);return false;
 }
 template<class Show,class Hide> void Apply(uintptr_t base,bool enabled,Show show,Hide hide){
  active=enabled;
  // Restore through native Show, which checks requested +94 and parent +98.
  // Hide affects actual +98 only; never overwrite the engine's fog visibility.
  for(auto it=suppressed.begin();it!=suppressed.end();){const auto hp=*it;++it;Update(base,hp,show,hide);}
 }
};
}
