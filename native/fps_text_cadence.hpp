#pragma once
#include <cstdint>
#include <cstring>
namespace wht {
inline const char* IntegerFpsText(const char* text,char* buffer,size_t capacity){
 if(!text||std::strncmp(text," FPS: ",6))return text;
 const char* end=text+6;while(*end>='0'&&*end<='9')++end;
 if(end==text+6||*end!='.')return text;
 const char* tail=end+1;while(*tail>='0'&&*tail<='9')++tail;
 while(*tail==' ')++tail;
 const size_t count=size_t(end-text);
 if(*tail||count>=capacity)return text;
 std::memcpy(buffer,text,count);buffer[count]=0;return buffer;
}

// Gate only stock FPS counter strings. Rendering and FPS measurements continue normally.
struct FpsTextCadence {
 struct Entry {uintptr_t font=0;uint64_t written=0;bool valid=false;};
 Entry entries[4]{};unsigned next=0;
 bool Allow(uintptr_t font,const char* text,uint64_t now){
  const bool fps=text&&!std::strncmp(text," FPS: ",6)&&text[6]>='0'&&text[6]<='9';
  Entry* found=nullptr;for(auto& e:entries)if(e.valid&&e.font==font){found=&e;break;}
  if(!fps){if(found)found->valid=false;return true;}
  if(!font)return true;
  if(found){if(now>=found->written&&now-found->written<1000)return false;found->written=now;return true;}
  for(auto& e:entries)if(!e.valid){found=&e;break;}
  if(!found)found=&entries[(next++)%4];
  *found={font,now,true};return true;
 }
};
inline bool AllowFpsText(uintptr_t font,const char* text,uint64_t now){static thread_local FpsTextCadence cadence;return cadence.Allow(font,text,now);}
}
