#pragma once
#include "network_ping.hpp"
#include "player_network129.hpp"
#include <MinHook.h>
namespace wht {
inline decltype(&send) sendOriginal129=nullptr;
inline decltype(&recv) recvOriginal129=nullptr;
inline decltype(&closesocket) closeOriginal129=nullptr;
inline int WSAAPI PingSend129(SOCKET s,const char* data,int size,int flags){
 const int result=sendOriginal129(s,data,size,flags),error=WSAGetLastError();
 try{network_ping::Observe(s,data,size,result);if(result>0)playerNetwork129.Observe(s,data,result,true);}catch(...){}
 WSASetLastError(error);return result;
}
inline int WSAAPI PingRecv129(SOCKET s,char* data,int size,int flags){
 const int result=recvOriginal129(s,data,size,flags),error=WSAGetLastError();
 try{if(result>0&&!(flags&MSG_PEEK)){network_ping::Received(s,data,result);playerNetwork129.Observe(s,data,result,false);}else if(result==0){network_ping::Closed(s);playerNetwork129.Closed(s);}}catch(...){}
 WSASetLastError(error);return result;
}
inline int WSAAPI PingClose129(SOCKET s){
 const int result=closeOriginal129(s),error=WSAGetLastError();
 try{if(result==0){network_ping::Closed(s);playerNetwork129.Closed(s);}}catch(...){}
 WSASetLastError(error);return result;
}
inline decltype(&WSARecv) wsaRecvOriginal129=nullptr;
inline decltype(&WSASend) wsaSendOriginal129=nullptr;
inline decltype(&GetQueuedCompletionStatus) completionOriginal129=nullptr;
inline int WSAAPI NetworkRecv129(SOCKET s,WSABUF* buffers,DWORD count,DWORD* bytes,DWORD* flags,OVERLAPPED* overlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE callback){
 const bool peek=flags&&(*flags&MSG_PEEK);try{if(overlapped&&!callback&&!peek)playerNetwork129.Track(s,buffers,count,overlapped);}catch(...){}
 const int result=wsaRecvOriginal129(s,buffers,count,bytes,flags,overlapped,callback),error=WSAGetLastError();
 try{if(result==SOCKET_ERROR&&error!=WSA_IO_PENDING)playerNetwork129.Cancel(overlapped);
  else if(result==0&&!overlapped&&!peek&&bytes){DWORD remaining=*bytes;for(DWORD i=0;i<count&&remaining;++i){DWORD part=(std::min)(remaining,buffers[i].len);playerNetwork129.Observe(s,buffers[i].buf,part,false);remaining-=part;}}
 }catch(...){}WSASetLastError(error);return result;
}
inline int WSAAPI NetworkSend129(SOCKET s,WSABUF* buffers,DWORD count,DWORD* bytes,DWORD flags,OVERLAPPED* overlapped,LPWSAOVERLAPPED_COMPLETION_ROUTINE callback){
 std::string copy;try{if(buffers&&count<=64)for(DWORD i=0;i<count;++i){if(buffers[i].len>65535-copy.size()){copy.clear();break;}copy.append(buffers[i].buf,buffers[i].len);}}catch(...){copy.clear();}
 const int result=wsaSendOriginal129(s,buffers,count,bytes,flags,overlapped,callback),error=WSAGetLastError();
 try{if(result==0||(result==SOCKET_ERROR&&error==WSA_IO_PENDING)){auto length=overlapped?copy.size():bytes?(std::min)(copy.size(),size_t(*bytes)):0;playerNetwork129.Observe(s,copy.data(),length,true);}}catch(...){}
 WSASetLastError(error);return result;
}
inline BOOL WINAPI NetworkCompletion129(HANDLE port,DWORD* bytes,ULONG_PTR* key,OVERLAPPED** overlapped,DWORD timeout){
 BOOL result=completionOriginal129(port,bytes,key,overlapped,timeout);DWORD error=GetLastError();
 try{if(overlapped&&*overlapped)playerNetwork129.Complete(*overlapped,bytes?*bytes:0,result!=FALSE);}catch(...){}
 SetLastError(error);return result;
}
inline bool InstallPing129(){
 const auto module=GetModuleHandleW(L"ws2_32.dll");if(!module)return false;
 void* targets[]={reinterpret_cast<void*>(GetProcAddress(module,"send")),reinterpret_cast<void*>(GetProcAddress(module,"recv")),reinterpret_cast<void*>(GetProcAddress(module,"closesocket")),reinterpret_cast<void*>(GetProcAddress(module,"WSARecv")),reinterpret_cast<void*>(GetProcAddress(module,"WSASend")),reinterpret_cast<void*>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"),"GetQueuedCompletionStatus"))};
 void* hooks[]={reinterpret_cast<void*>(PingSend129),reinterpret_cast<void*>(PingRecv129),reinterpret_cast<void*>(PingClose129),reinterpret_cast<void*>(NetworkRecv129),reinterpret_cast<void*>(NetworkSend129),reinterpret_cast<void*>(NetworkCompletion129)};
 void** originals[]={reinterpret_cast<void**>(&sendOriginal129),reinterpret_cast<void**>(&recvOriginal129),reinterpret_cast<void**>(&closeOriginal129),reinterpret_cast<void**>(&wsaRecvOriginal129),reinterpret_cast<void**>(&wsaSendOriginal129),reinterpret_cast<void**>(&completionOriginal129)};
 unsigned made=0;
 for(;made<6;++made)if(!targets[made]||MH_CreateHook(targets[made],hooks[made],originals[made])!=MH_OK)break;
 if(made==6){unsigned enabled=0;for(;enabled<6;++enabled)if(MH_EnableHook(targets[enabled])!=MH_OK)break;if(enabled==6)return true;
  for(unsigned i=0;i<enabled;++i)MH_DisableHook(targets[i]);}
 for(unsigned i=0;i<made;++i)MH_RemoveHook(targets[i]);
 return false;
}
}
