#pragma once
#include "self_cast131.hpp"
#include "all_skills131.hpp"
namespace wht {
template<class Read,class Points,class Data,class Click> bool LearnSlot131(unsigned slot,Read read,Points points,Data data,Click click){
 if(slot>=11)return false;
 SelfCastSource131 source{},current{};if(!read(source)||points(source.unit)<=0)return false;
 const auto same=[&](){return read(current)&&SameSelfCastSource131(source,current);};
 const bool opened=data(11).order==AllSkills131::back;
 if(!opened){bool sent=false;for(unsigned i=0;i<12;++i)if(data(i).order==AllSkills131::plus){sent=click(i);break;}
  if(!sent)return false;if(!same()||data(11).order!=AllSkills131::back)return true;
 }
 const auto value=data(slot);
 if(same()&&value.ability==AllSkills131::heroAbility&&value.order&&value.order!=AllSkills131::plus&&value.order!=AllSkills131::back)click(slot);
 if(!opened&&same()&&data(11).order==AllSkills131::back)click(11);
 return true;
}
}
