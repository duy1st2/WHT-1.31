#pragma once
#include "network_ping.hpp"
#include <MinHook.h>
namespace wht {
inline decltype(&send) sendOriginal129=nullptr;
inline decltype(&recv) recvOriginal129=nullptr;
inline decltype(&closesocket) closeOriginal129=nullptr;
inline int WSAAPI PingSend129(SOCKET s,const char* data,int size,int flags){
 const int result=sendOriginal129(s,data,size,flags),error=WSAGetLastError();
 try{network_ping::Observe(s,data,size,result);}catch(...){}
 WSASetLastError(error);return result;
}
inline int WSAAPI PingRecv129(SOCKET s,char* data,int size,int flags){
 const int result=recvOriginal129(s,data,size,flags),error=WSAGetLastError();
 try{if(result>0&&!(flags&MSG_PEEK))network_ping::Received(s,data,result);else if(result==0)network_ping::Closed(s);}catch(...){}
 WSASetLastError(error);return result;
}
inline int WSAAPI PingClose129(SOCKET s){
 const int result=closeOriginal129(s),error=WSAGetLastError();
 try{if(result==0)network_ping::Closed(s);}catch(...){}
 WSASetLastError(error);return result;
}
inline bool InstallPing129(){
 const auto module=GetModuleHandleW(L"ws2_32.dll");if(!module)return false;
 void* targets[]={reinterpret_cast<void*>(GetProcAddress(module,"send")),reinterpret_cast<void*>(GetProcAddress(module,"recv")),reinterpret_cast<void*>(GetProcAddress(module,"closesocket"))};
 void* hooks[]={reinterpret_cast<void*>(PingSend129),reinterpret_cast<void*>(PingRecv129),reinterpret_cast<void*>(PingClose129)};
 void** originals[]={reinterpret_cast<void**>(&sendOriginal129),reinterpret_cast<void**>(&recvOriginal129),reinterpret_cast<void**>(&closeOriginal129)};
 unsigned made=0;
 for(;made<3;++made)if(!targets[made]||MH_CreateHook(targets[made],hooks[made],originals[made])!=MH_OK)break;
 if(made==3){unsigned enabled=0;for(;enabled<3;++enabled)if(MH_EnableHook(targets[enabled])!=MH_OK)break;if(enabled==3)return true;
  for(unsigned i=0;i<enabled;++i)MH_DisableHook(targets[i]);}
 for(unsigned i=0;i<made;++i)MH_RemoveHook(targets[i]);
 return false;
}
}
