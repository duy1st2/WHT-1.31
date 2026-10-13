#pragma once
#include <string>
#include <cstring>
namespace wht {
inline bool IsMapScript129(const char* path){
 if(!path)return false;size_t n=0;for(;n<1024&&path[n];++n){}if(n==1024)return false;
 auto start=path;for(auto p=path;*p;++p)if(*p=='\\'||*p=='/')start=p+1;return _stricmp(start,"war3map.j")==0;
}
inline unsigned PatchDefeatCalls129(char* data,size_t size){
 unsigned changed=0;if(!data||size>64*1024*1024)return 0;
 for(size_t begin=0;begin<size;){
  size_t end=begin;while(end<size&&data[end]!='\r'&&data[end]!='\n')++end;
  size_t at=begin;while(at<end&&(data[at]==' '||data[at]=='\t'))++at;
  if(end-at>=5&&!memcmp(data+at,"call",4)&&(data[at+4]==' '||data[at+4]=='\t')){
   size_t fn=at+4;while(fn<end&&(data[fn]==' '||data[fn]=='\t'))++fn;
   for(const char* name:{"EndGame","CustomDefeatBJ"}){auto n=strlen(name);if(end-fn<n||memcmp(data+fn,name,n))continue;size_t after=fn+n;while(after<end&&(data[after]==' '||data[after]=='\t'))++after;
    if(after<end&&data[after]=='('){data[at]='/';data[at+1]='/';++changed;}break;
   }
  }
  begin=end;while(begin<size&&(data[begin]=='\r'||data[begin]=='\n'))++begin;
 }
 return changed;
}
struct AmbientLight129 {
 using Get=HRESULT(WINAPI*)(uintptr_t,DWORD,DWORD*);
 using Set=HRESULT(WINAPI*)(uintptr_t,DWORD,DWORD);
 uintptr_t device=0;DWORD saved=0,written=0;bool active=false;
 bool Apply(uintptr_t renderer,unsigned requested){
  if(requested>255)return false;if(!requested&&!active)return true;
  __try{
   const auto current=*reinterpret_cast<uintptr_t*>(renderer+0x59c);if(!current){*this={};return false;}
   if(device!=current){*this={};device=current;}
   auto table=*reinterpret_cast<uintptr_t*>(device);
   const auto get=reinterpret_cast<Get>(*reinterpret_cast<uintptr_t*>(table+58*4));
   const auto set=reinterpret_cast<Set>(*reinterpret_cast<uintptr_t*>(table+57*4));
   if(!get||!set)return false;DWORD value=0;if(FAILED(get(device,139,&value)))return false;
   if(!requested){if(active&&value==written&&value!=saved&&FAILED(set(device,139,saved)))return false;active=false;return true;}
   if(!active||value!=written)saved=value;DWORD desired=0xff000000|(requested<<16)|(requested<<8)|requested;
   if(value!=desired&&FAILED(set(device,139,desired)))return false;written=desired;active=true;return true;
  }__except(EXCEPTION_EXECUTE_HANDLER){*this={};return false;}
 }
};
}
