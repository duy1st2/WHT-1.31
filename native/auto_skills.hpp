#pragma once
#include <array>
#include <unordered_map>
#include <cstdint>
namespace wht {
struct AutoSkillsState {
 struct Sequence {std::array<uint32_t,9> orders{};unsigned cursor=0;};
 std::unordered_map<uint64_t,Sequence> sequences;
 bool running=false;uint64_t selected=0;uint64_t next=0;
 unsigned scan=0;uint32_t target=0,deadTarget=0;float nearest=0,deadNearest=0;bool targetDead=false;
 void Reset(){*this={};}
 static int Number(unsigned vk){if(vk>='1'&&vk<='9')return int(vk-'1');if(vk>=0x61&&vk<=0x69)return int(vk-0x61);return -1;}
 bool Bind(uint64_t unit,int slot,uint32_t order){if(!unit||slot<0||slot>=9||!order)return false;auto& s=sequences[unit];s.orders[slot]=order;s.cursor=0;return true;}
 uint32_t Current(uint64_t unit){auto found=sequences.find(unit);if(found==sequences.end())return 0;auto& s=found->second;for(unsigned n=0;n<9;++n){if(s.orders[s.cursor])return s.orders[s.cursor];s.cursor=(s.cursor+1)%9;}return 0;}
 void Advance(uint64_t unit){auto found=sequences.find(unit);if(found!=sequences.end())found->second.cursor=(found->second.cursor+1)%9;}
};
}
