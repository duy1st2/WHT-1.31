#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <mswsock.h>
#include <MinHook.h>
#include "host_moderation131.hpp"
#include "resource131.hpp"
#include <mutex>
#include <unordered_map>
#include <shellapi.h>
namespace wht {
inline void ModerationLog131(const std::wstring& folder,const std::string& text){Log(folder,"pid="+std::to_string(GetCurrentProcessId())+" "+text);}
inline bool ModerationLocalName131(uintptr_t base,unsigned build,char* out){
 const auto* p=ResourceProfileFor131(build);if(!p||!ResourceCode131(base,*p))return false;
 __try{unsigned local=24;auto world=ResourceWorld131(base,*p,local);if(!world)return false;ResourceNative131 api{reinterpret_cast<decltype(api.player)>(base+p->playerFn),reinterpret_cast<decltype(api.integer)>(base+p->integerFn),reinterpret_cast<decltype(api.string)>(base+p->stringFn)};auto player=api.player(world,local);if(!ResourceIdentity131(player,base,p->playerTable))return false;
  const char* name=api.string(player+p->nameOffset);if(!name)return false;unsigned n=0;for(;n<64&&name[n];++n)out[n]=name[n];if(!n||n==64)return false;out[n]=0;return true;
 }__except(1){return false;}
}
inline bool ModerationCopy131(void* target,const void* source,size_t size){if(!size)return true;__try{memcpy(target,source,size);return true;}__except(1){return false;}}
inline thread_local unsigned moderationDepth131=0;
struct ModerationDepth131 {bool outer;ModerationDepth131():outer(moderationDepth131++==0){}~ModerationDepth131(){--moderationDepth131;}};
struct ModerationEpoch131 {uint64_t room=0,socket=0;};
struct PlayerNetwork131 {
 // Counts only: never log names, endpoints, packet contents or chat.
 unsigned slotReports=0,rejectReports=0;
 uint64_t acceptRequests=0;
 uint64_t slotAttempts=0,slotRejected=0,nonGameFrames=0;
 uint64_t observedBytes=0,joinsIn=0,joinsOut=0,slotJoinsOut=0,playerInfoOut=0,invalidFrames=0,unsupportedIo=0,identityResets=0;
 std::string Status(){std::lock_guard<std::mutex> lock(mutex);return std::string("observer=")+(ready?"ready":"unavailable")+", active="+std::to_string(enabled)+", acceptex="+std::to_string(acceptRequests)+", bytes="+std::to_string(observedBytes)+", join-in="+std::to_string(joinsIn)+", join-out="+std::to_string(joinsOut)+", slotjoin-out="+std::to_string(slotJoinsOut)+", playerinfo-out="+std::to_string(playerInfoOut)+", slot-attempts="+std::to_string(slotAttempts)+", slot-rejected="+std::to_string(slotRejected)+", non-game="+std::to_string(nonGameFrames)+", rejected="+std::to_string(invalidFrames)+", unsupported-io="+std::to_string(unsupportedIo)+", identity-resets="+std::to_string(identityResets)+", peers="+std::to_string(model.peers.size())+", host="+std::to_string(model.host)+", identity-known="+std::to_string(!model.ownName.empty())+", local-id="+std::to_string(model.ownId)+", proof="+std::to_string(model.Ready());}

 struct Pending {uintptr_t socket=0;ModerationEpoch131 epoch;bool sent=false;std::string copy;std::vector<WSABUF> buffers;};
 std::mutex mutex;uint64_t roomEpoch=1;std::unordered_map<uintptr_t,uint64_t> socketEpochs;HostModeration131 model;bool enabled=false,ready=false;uint64_t nextBanScan=0;std::wstring folder;
 std::unordered_map<uintptr_t,std::vector<unsigned char>> input,output;
 std::unordered_map<OVERLAPPED*,Pending> pending;
 void Configure(bool allowed,const std::wstring& dir,const std::string& own){std::lock_guard<std::mutex> lock(mutex);folder=dir;enabled=allowed&&ready;if(!enabled){++roomEpoch;model.Reset();input.clear();output.clear();pending.clear();}else if(!own.empty()&&own!=model.ownName){const bool changed=!model.ownName.empty();if(changed){++identityResets;ModerationLog131(folder,"WHT: moderation local identity changed; candidate room and peer mapping cleared.");++roomEpoch;}model.Identity(own);if(changed){input.clear();output.clear();pending.clear();}else ModerationLog131(folder,"WHT: moderation first native/local identity acquired; captured room handshake preserved.");}}
 ModerationEpoch131 Ticket(uintptr_t socket)noexcept{try{std::lock_guard<std::mutex> lock(mutex);if(socketEpochs.size()>=4096&&!socketEpochs.count(socket)){++roomEpoch;socketEpochs.clear();model.Reset();input.clear();output.clear();pending.clear();}return {roomEpoch,socketEpochs[socket]};}catch(...){return {~uint64_t(0),0};}}
 void Invalidate(uintptr_t socket){if(model.peers.count(socket)){++roomEpoch;model.Reset();input.clear();output.clear();pending.clear();}else{++socketEpochs[socket];input.erase(socket);output.erase(socket);for(auto i=pending.begin();i!=pending.end();)if(i->second.socket==socket)i=pending.erase(i);else ++i;}}

 void Observe(uintptr_t socket,const char* data,size_t length,bool sent,ModerationEpoch131 epoch={}){
  std::lock_guard<std::mutex> lock(mutex);if(!enabled||!length||!data||(epoch.room&&(epoch.room!=roomEpoch||epoch.socket!=socketEpochs[socket])))return;
  observedBytes+=length;
  auto& streams=sent?output:input;if(streams.size()>=256&&!streams.count(socket))return;
  auto& stream=streams[socket];if(length>65535||stream.size()+length>131070){Invalidate(socket);return;}
  const auto previous=stream.size();stream.resize(previous+length);if(!ModerationCopy131(stream.data()+previous,data,length)){Invalidate(socket);return;}size_t consumed=0;
  while(stream.size()-consumed>=4){const auto* p=stream.data()+consumed;const unsigned count=p[2]|unsigned(p[3])<<8;
   if(p[0]!=0xf7||count<4){++invalidFrames;++nonGameFrames;if(model.peers.count(socket)&&rejectReports++<16)ModerationLog131(folder,"WHT: moderation peer stream rejected: invalid framing; candidate room proof cleared.");Invalidate(socket);return;}if(count>stream.size()-consumed)break;
   const bool slotPacket=p[1]==4||p[1]==9;
   if(slotPacket){++slotAttempts;if(slotReports++<16){const unsigned info=count>=6?p[4]|unsigned(p[5])<<8:0,rows=count>=7?p[6]:0;ModerationLog131(folder,"WHT: moderation slot schema: type="+std::to_string(p[1])+", sent="+std::to_string(sent)+", packet-bytes="+std::to_string(count)+", info-bytes="+std::to_string(info)+", rows="+std::to_string(rows)+", expected-nine-byte-info="+std::to_string(7+9*rows)+", recorded-peer="+std::to_string(model.peers.count(socket))+", client="+std::to_string(model.client));}}
   if(!model.Packet(socket,p,count,sent)){++invalidFrames;if(slotPacket)++slotRejected;if(rejectReports++<16)ModerationLog131(folder,"WHT: moderation parser rejected: type="+std::to_string(p[1])+", sent="+std::to_string(sent)+", packet-bytes="+std::to_string(count)+", recorded-peer="+std::to_string(model.peers.count(socket))+"; candidate proof cleared.");Invalidate(socket);return;}
   if(p[1]==0x1e){if(sent)++joinsOut;else ++joinsIn;}if(sent&&p[1]==4)++slotJoinsOut;if(sent&&p[1]==6)++playerInfoOut;consumed+=count;}
  if(consumed)stream.erase(stream.begin(),stream.begin()+consumed);
 }
 void Closed(uintptr_t socket){++socketEpochs[socket];model.Closed(socket);input.erase(socket);output.erase(socket);for(auto i=pending.begin();i!=pending.end();)if(i->second.socket==socket)i=pending.erase(i);else ++i;if(model.peers.empty()){++roomEpoch;model.Reset();input.clear();output.clear();pending.clear();}}
 void Eof(uintptr_t socket,ModerationEpoch131 epoch){std::lock_guard<std::mutex> lock(mutex);if(!epoch.room||(epoch.room==roomEpoch&&epoch.socket==socketEpochs[socket]))Closed(socket);}
 void Unsupported(uintptr_t socket){std::lock_guard<std::mutex> lock(mutex);++unsupportedIo;Invalidate(socket);}
 void Track(SOCKET socket,WSABUF* buffers,DWORD count,OVERLAPPED* overlap,bool sent){
  std::lock_guard<std::mutex> lock(mutex);if(!enabled||!overlap||!buffers||!count||count>64||pending.size()>=1024){Invalidate(socket);return;}
  if(pending.count(overlap)){Invalidate(socket);pending.erase(overlap);return;}
  WSABUF snapshot[64]{};if(!ModerationCopy131(snapshot,buffers,count*sizeof(WSABUF))){Invalidate(socket);return;}
  Pending value;value.socket=socket;value.epoch={roomEpoch,socketEpochs[socket]};value.sent=sent;size_t total=0;
  for(DWORD i=0;i<count;++i){const auto buffer=snapshot[i];if(buffer.len>65535-total){Invalidate(socket);return;}if(sent){value.copy.resize(total+buffer.len);if(!ModerationCopy131(value.copy.data()+total,buffer.buf,buffer.len)){Invalidate(socket);return;}}else value.buffers.push_back(buffer);total+=buffer.len;}
  pending[overlap]=std::move(value);
 }
 void Complete(OVERLAPPED* overlap,DWORD bytes,bool success){Pending value;
  {std::lock_guard<std::mutex> lock(mutex);auto i=pending.find(overlap);if(i==pending.end())return;value=std::move(i->second);pending.erase(i);}
  if(!success){std::lock_guard<std::mutex> lock(mutex);if(value.epoch.room==roomEpoch&&value.epoch.socket==socketEpochs[value.socket])Invalidate(value.socket);return;}
  if(value.sent){if(bytes>value.copy.size()){Unsupported(value.socket);return;}Observe(value.socket,value.copy.data(),bytes,true,value.epoch);}
  else{if(!bytes){Eof(value.socket,value.epoch);return;}for(const auto& buffer:value.buffers){const auto size=(std::min)(bytes,buffer.len);if(size)Observe(value.socket,buffer.buf,size,false,value.epoch);bytes-=size;if(!bytes)break;}if(bytes)Unsupported(value.socket);}
 }
 bool Banned(const std::string& name){wchar_t reason[256]{};return GetPrivateProfileStringW(L"Banned List",Wide(name).c_str(),L"",reason,256,(folder+L"\\TS-Banned.ini").c_str())>0;}
 static bool Disconnect(uintptr_t socket){ModerationDepth131 depth;return shutdown(SOCKET(socket),SD_BOTH)==0;}
 void Enforce(){std::lock_guard<std::mutex> lock(mutex);const auto now=GetTickCount64();if(!enabled||now<nextBanScan)return;nextBanScan=now+250;model.Enforce([&](const std::string& name){return Banned(name);},Disconnect);}
 std::string Act(const ModerationCommand131& command,bool ban){std::lock_guard<std::mutex> lock(mutex);if(!enabled)return "[WHT] Kick: network observer unavailable or unauthorized; no action taken.";
  return model.Act(Utf8(command.target),ban,Disconnect,[&](const std::string& name){return name.find_first_of("=\r\n[]")==name.npos&&SaveSetting(folder+L"\\TS-Banned.ini",L"Banned List",Wide(name).c_str(),command.reason);});}
};
inline PlayerNetwork131 playerNetwork131;
inline decltype(&send) moderationSend131=nullptr;inline decltype(&recv) moderationRecv131=nullptr;inline decltype(&closesocket) moderationClose131=nullptr;
inline decltype(&WSASend) moderationWsaSend131=nullptr;inline decltype(&WSARecv) moderationWsaRecv131=nullptr;
inline decltype(&GetQueuedCompletionStatus) moderationCompletion131=nullptr;
inline decltype(&GetQueuedCompletionStatusEx) moderationCompletionEx131=nullptr;
inline decltype(&WSAGetOverlappedResult) moderationResult131=nullptr;
inline LPFN_ACCEPTEX moderationAcceptEx131=nullptr;
inline LPFN_ACCEPTEX ModerationAcceptAddress131(){
 const SOCKET probe=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);if(probe==INVALID_SOCKET)return nullptr;
 GUID id=WSAID_ACCEPTEX;LPFN_ACCEPTEX address=nullptr;DWORD bytes=0;const int result=WSAIoctl(probe,SIO_GET_EXTENSION_FUNCTION_POINTER,&id,sizeof(id),&address,sizeof(address),&bytes,nullptr,nullptr);closesocket(probe);return result==0?address:nullptr;
}
inline BOOL PASCAL ModerationAcceptEx131(SOCKET listener,SOCKET accepted,PVOID buffer,DWORD receiveBytes,DWORD localBytes,DWORD remoteBytes,LPDWORD bytes,LPOVERLAPPED overlap){
 const int prior=WSAGetLastError();ModerationDepth131 depth;
 if(depth.outer)try{{std::lock_guard<std::mutex> lock(playerNetwork131.mutex);++playerNetwork131.acceptRequests;}if(receiveBytes){WSABUF part{receiveBytes,static_cast<char*>(buffer)};playerNetwork131.Track(accepted,&part,1,overlap,false);}}catch(...){}
 WSASetLastError(prior);const BOOL result=moderationAcceptEx131(listener,accepted,buffer,receiveBytes,localBytes,remoteBytes,bytes,overlap);const int error=WSAGetLastError();
 if(depth.outer&&!result&&error!=ERROR_IO_PENDING)try{playerNetwork131.Complete(overlap,0,false);}catch(...){}
 WSASetLastError(error);return result;
}
inline int WSAAPI ModerationSend131(SOCKET s,const char* data,int length,int flags){const int prior=WSAGetLastError();ModerationDepth131 depth;const auto epoch=depth.outer?playerNetwork131.Ticket(s):ModerationEpoch131{};WSASetLastError(prior);const int result=moderationSend131(s,data,length,flags),error=WSAGetLastError();if(depth.outer)try{if(result>0)playerNetwork131.Observe(s,data,result,true,epoch);}catch(...){}WSASetLastError(error);return result;}
inline int WSAAPI ModerationRecv131(SOCKET s,char* data,int length,int flags){const int prior=WSAGetLastError();ModerationDepth131 depth;const auto epoch=depth.outer?playerNetwork131.Ticket(s):ModerationEpoch131{};WSASetLastError(prior);const int result=moderationRecv131(s,data,length,flags),error=WSAGetLastError();if(depth.outer)try{if(result>0&&!(flags&MSG_PEEK))playerNetwork131.Observe(s,data,result,false,epoch);else if(!result)playerNetwork131.Eof(s,epoch);}catch(...){}WSASetLastError(error);return result;}
inline int WSAAPI ModerationClose131(SOCKET s){const int prior=WSAGetLastError();ModerationDepth131 depth;if(depth.outer)try{std::lock_guard<std::mutex> lock(playerNetwork131.mutex);playerNetwork131.Closed(s);}catch(...){}WSASetLastError(prior);const int result=moderationClose131(s),error=WSAGetLastError();WSASetLastError(error);return result;}

inline void ModerationBuffers131(SOCKET socket,WSABUF* buffers,DWORD count,DWORD bytes,bool sent,ModerationEpoch131 epoch={}){if(!sent&&!bytes){playerNetwork131.Eof(socket,epoch);return;}if(!buffers||count>64){playerNetwork131.Unsupported(socket);return;}for(DWORD i=0;i<count&&bytes;++i){DWORD part=(std::min)(bytes,buffers[i].len);playerNetwork131.Observe(socket,buffers[i].buf,part,sent,epoch);bytes-=part;}if(bytes)playerNetwork131.Unsupported(socket);}
inline int WSAAPI ModerationWsaSend131(SOCKET s,WSABUF* buffers,DWORD count,DWORD* bytes,DWORD flags,OVERLAPPED* overlap,LPWSAOVERLAPPED_COMPLETION_ROUTINE callback){const int prior=WSAGetLastError();ModerationDepth131 depth;const auto epoch=depth.outer?playerNetwork131.Ticket(s):ModerationEpoch131{};
 if(depth.outer)try{if(overlap&&!callback)playerNetwork131.Track(s,buffers,count,overlap,true);else if(callback)playerNetwork131.Unsupported(s);}catch(...){}
 WSASetLastError(prior);const int result=moderationWsaSend131(s,buffers,count,bytes,flags,overlap,callback),error=WSAGetLastError();
 if(depth.outer)try{if(!overlap&&result==0&&bytes)ModerationBuffers131(s,buffers,count,*bytes,true,epoch);else if(overlap&&result==SOCKET_ERROR&&error!=WSA_IO_PENDING)playerNetwork131.Complete(overlap,0,false);}catch(...){}WSASetLastError(error);return result;}
inline int WSAAPI ModerationWsaRecv131(SOCKET s,WSABUF* buffers,DWORD count,DWORD* bytes,DWORD* flags,OVERLAPPED* overlap,LPWSAOVERLAPPED_COMPLETION_ROUTINE callback){const int prior=WSAGetLastError();ModerationDepth131 depth;const auto epoch=depth.outer?playerNetwork131.Ticket(s):ModerationEpoch131{};const bool peek=flags&&(*flags&MSG_PEEK);
 if(depth.outer&&!peek)try{if(overlap&&!callback)playerNetwork131.Track(s,buffers,count,overlap,false);else if(callback)playerNetwork131.Unsupported(s);}catch(...){}
 WSASetLastError(prior);const int result=moderationWsaRecv131(s,buffers,count,bytes,flags,overlap,callback),error=WSAGetLastError();
 if(depth.outer&&!peek)try{if(!overlap&&result==0&&bytes)ModerationBuffers131(s,buffers,count,*bytes,false,epoch);else if(overlap&&result==SOCKET_ERROR&&error!=WSA_IO_PENDING)playerNetwork131.Complete(overlap,0,false);}catch(...){}WSASetLastError(error);return result;}
inline BOOL WINAPI ModerationCompletion131(HANDLE port,DWORD* bytes,ULONG_PTR* key,OVERLAPPED** overlap,DWORD timeout){const BOOL result=moderationCompletion131(port,bytes,key,overlap,timeout);const DWORD error=GetLastError();try{if(overlap&&*overlap)playerNetwork131.Complete(*overlap,bytes?*bytes:0,result!=FALSE);}catch(...){}SetLastError(error);return result;}
inline BOOL WINAPI ModerationCompletionEx131(HANDLE port,LPOVERLAPPED_ENTRY entries,ULONG count,ULONG* removed,DWORD timeout,BOOL alertable){const BOOL result=moderationCompletionEx131(port,entries,count,removed,timeout,alertable);const DWORD error=GetLastError();try{if(result&&entries&&removed&&*removed<=count)for(ULONG i=0;i<*removed;++i)playerNetwork131.Complete(entries[i].lpOverlapped,entries[i].dwNumberOfBytesTransferred,entries[i].Internal==0);}catch(...){}SetLastError(error);return result;}
inline BOOL WSAAPI ModerationResult131(SOCKET socket,OVERLAPPED* overlap,DWORD* bytes,BOOL wait,DWORD* flags){const BOOL result=moderationResult131(socket,overlap,bytes,wait,flags);const int error=WSAGetLastError();try{if(result)playerNetwork131.Complete(overlap,bytes?*bytes:0,true);else if(error!=WSA_IO_INCOMPLETE)playerNetwork131.Complete(overlap,0,false);}catch(...){}WSASetLastError(error);return result;}
// Disabled until actual 1.31 lobby transport and peer ownership are validated.
inline constexpr bool moderationRuntimeVerified131=false;
// Installing observers never authorizes a disconnect or automatic enforcement.
inline bool InstallModeration131(){
 auto status=MH_Initialize();if(status!=MH_OK&&status!=MH_ERROR_ALREADY_INITIALIZED)return false;
 const auto ws=GetModuleHandleW(L"ws2_32.dll"),kernel=GetModuleHandleW(L"kernel32.dll");if(!ws||!kernel)return false;
 const auto acceptAddress=ModerationAcceptAddress131();if(!acceptAddress)return false;
 const char* names[]={"send","recv","closesocket","WSASend","WSARecv","GetQueuedCompletionStatus","GetQueuedCompletionStatusEx","WSAGetOverlappedResult","AcceptEx"};
 void* hooks[]={reinterpret_cast<void*>(ModerationSend131),reinterpret_cast<void*>(ModerationRecv131),reinterpret_cast<void*>(ModerationClose131),reinterpret_cast<void*>(ModerationWsaSend131),reinterpret_cast<void*>(ModerationWsaRecv131),reinterpret_cast<void*>(ModerationCompletion131),reinterpret_cast<void*>(ModerationCompletionEx131),reinterpret_cast<void*>(ModerationResult131),reinterpret_cast<void*>(ModerationAcceptEx131)};
 void** originals[]={reinterpret_cast<void**>(&moderationSend131),reinterpret_cast<void**>(&moderationRecv131),reinterpret_cast<void**>(&moderationClose131),reinterpret_cast<void**>(&moderationWsaSend131),reinterpret_cast<void**>(&moderationWsaRecv131),reinterpret_cast<void**>(&moderationCompletion131),reinterpret_cast<void**>(&moderationCompletionEx131),reinterpret_cast<void**>(&moderationResult131),reinterpret_cast<void**>(&moderationAcceptEx131)};
 void* targets[9]{};unsigned made=0,enabled=0;for(;made<9;++made){targets[made]=made==8?reinterpret_cast<void*>(acceptAddress):reinterpret_cast<void*>(GetProcAddress(made==5||made==6?kernel:ws,names[made]));if(!targets[made]||MH_CreateHook(targets[made],hooks[made],originals[made])!=MH_OK)break;}
 if(made==9){for(;enabled<9;++enabled)if(MH_EnableHook(targets[enabled])!=MH_OK)break;if(enabled==9)return true;}
 for(unsigned i=0;i<enabled;++i)MH_DisableHook(targets[i]);for(unsigned i=0;i<made;++i)MH_RemoveHook(targets[i]);return false;
}
inline std::string ModerationRegistryName131(){wchar_t name[256]{};DWORD bytes=sizeof(name);if(RegGetValueW(HKEY_CURRENT_USER,L"Software\\Blizzard Entertainment\\Warcraft III",L"userlocal",RRF_RT_REG_SZ,nullptr,name,&bytes)!=ERROR_SUCCESS)return {};return Utf8(name);}
inline void UpdateModeration131(uintptr_t base,unsigned build,uintptr_t ui,bool allowed,const Config&,const std::wstring& folder){
 static ULONGLONG retry=0;const auto now=GetTickCount64();if(allowed&&!playerNetwork131.ready&&now>=retry){retry=now+5000;playerNetwork131.ready=InstallModeration131();ModerationLog131(folder,playerNetwork131.ready?"WHT: moderation observation-only hooks ready; kick/ban and automatic enforcement remain disabled.":"WHT: host moderation transport hooks unavailable; no disconnects enabled.");}
 char name[65]{};std::string own;if(ui&&ModerationLocalName131(base,build,name))own=name;else own=ModerationRegistryName131();
 playerNetwork131.Configure(allowed,folder,own);if(moderationRuntimeVerified131)playerNetwork131.Enforce();
 static ULONGLONG nextStatus=0;static std::string previousStatus;if(allowed&&now>=nextStatus){nextStatus=now+5000;auto status=playerNetwork131.Status();if(status!=previousStatus){previousStatus=status;ModerationLog131(folder,"WHT: moderation diagnostic (observation only): "+status);}}
 static bool previous=false;bool host=false;{std::lock_guard<std::mutex> lock(playerNetwork131.mutex);host=playerNetwork131.enabled&&playerNetwork131.model.Ready();}if(host!=previous){previous=host;ModerationLog131(folder,host?"WHT: moderation candidate handshake observed; native 1.31 validation still required; disconnects disabled.":"WHT: host moderation proof cleared; disconnects unavailable until reverified.");}
}
inline bool ModerationCommandName131(const std::wstring& name){return name==L"#kick"||name==L"#ban"||name==L"#openbl";}
inline std::string ExecuteModeration131(HWND window,const std::wstring& folder,const std::wstring& name,const std::wstring& args){
 ModerationCommand131 command;if(!ParseModeration131(name,args,command))return name==L"#ban"?"[WHT] Usage: #ban <1..24 or exact name> [reason]":name==L"#kick"?"[WHT] Usage: #kick <1..24 or exact name>":"[WHT] Usage: #openbl";
 if(name!=L"#openbl"){if(!moderationRuntimeVerified131){ModerationLog131(folder,"WHT: moderation request refused (observation only): "+playerNetwork131.Status());return "[WHT] Kick/Ban unavailable: Warcraft III 1.31 lobby transport is not verified; no action taken.";}return playerNetwork131.Act(command,name==L"#ban");}
 const auto path=folder+L"\\TS-Banned.ini";if(GetFileAttributesW(path.c_str())==INVALID_FILE_ATTRIBUTES){const HANDLE file=CreateFileW(path.c_str(),GENERIC_WRITE,FILE_SHARE_READ,nullptr,CREATE_NEW,FILE_ATTRIBUTE_NORMAL,nullptr);if(file==INVALID_HANDLE_VALUE)return "[WHT] Ban list could not be created.";const char header[]="[Banned List]\r\n";DWORD written=0;const bool ok=WriteFile(file,header,sizeof(header)-1,&written,nullptr)&&written==sizeof(header)-1;CloseHandle(file);if(!ok)return "[WHT] Ban list could not be written.";}
 return reinterpret_cast<INT_PTR>(ShellExecuteW(window,L"open",path.c_str(),nullptr,folder.c_str(),SW_SHOWNORMAL))>32?"[WHT] Ban list opened.":"[WHT] Ban list could not be opened.";
}
}
