#pragma once
#include "goku_pick131.hpp"
#include "resource_text131.hpp"
namespace wht {
template<class Repair> unsigned RepairMthLabels131(const std::array<GokuIcon131*,12>& buttons,Repair repair){
 unsigned changed=0;for(auto* button:buttons)if(button&&button->shown&&button->label)changed+=repair(button->label);return changed;
}
struct MthText131 {
 uintptr_t checkedBase=0;unsigned checkedBuild=0;bool ready=false,reported=false;
 void Update(uintptr_t base,unsigned build,const std::wstring& folder,const std::array<GokuIcon131*,12>& buttons){
  bool visible=false;for(auto* button:buttons)if(button&&button->shown&&button->label){visible=true;break;}if(!visible)return;
  const auto* profile=ResourceTextProfileFor131(build);
  if(checkedBase!=base||checkedBuild!=build){checkedBase=base;checkedBuild=build;reported=false;ready=profile&&ResourceTextGuard131(base,*profile);Log(folder,ready?"WHT: MTH glyph projection profile verified.":"WHT: MTH glyph projection profile refused.");}
  if(!ready||!profile)return;
  // Engine refresh can replace glyph origin without changing frame bounds.
  // The verified helper only invokes native mesh positioning for wrong origins.
  const auto repaired=RepairMthLabels131(buttons,[&](void* font){return RepairResourceFont131(base,*profile,font);});
  if(repaired&&!reported){reported=true;Log(folder,"WHT: MTH button glyph origins repaired: "+std::to_string(repaired));}
 }
};
}
