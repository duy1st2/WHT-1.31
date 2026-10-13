#pragma once
#include "kick_player129.hpp"
#include "common.hpp"
#include <winsock2.h>
#include <unordered_map>
#include <mutex>
namespace wht {
struct PlayerNetwork129 {
 struct Pending {SOCKET socket;std::vector<WSABUF> buffers;};
 std::mutex mutex;
 KickPlayers129 players;
 std::unordered_map<SOCKET,std::vector<unsigned char>> incoming,outgoing;
 std::unordered_map<OVERLAPPED*,Pending> pending;
 std::array<std::string,256> names{};
 std::array<unsigned char,24> ids{},humans{};
 std::unordered_map<SOCKET,std::string> peerNames;
 std::string ownName;
 std::array<std::string,4> ownAliases{};
 unsigned localId=0;bool host=false,enabled=false,notifications=true;
 std::wstring folder,newName,hostName;
 std::vector<std::string> events;
 void Event(const std::string& value){if(events.size()<128)events.push_back(value);}
 bool Banned(const std::string& name)const{
  wchar_t reason[256]{};return GetPrivateProfileStringW(L"Banned List",Wide(name).c_str(),L"",reason,256,(folder+L"\\TS-Banned.ini").c_str())>0;
 }
 static bool Disconnect(uintptr_t socket){return shutdown(SOCKET(socket),SD_BOTH)==0;}
 void Enforce(){if(enabled&&host&&localId)players.EnforceBans([&](const std::string& n){return Banned(n);},Disconnect);}
 void Rows(){for(unsigned i=0;i<24;++i)players.Row(i,names[ids[i]],humans[i]!=0,ids[i]==localId,host);Enforce();}
 static bool PacketName(const unsigned char* data,size_t size,size_t start,std::string& name){
  if(start>=size)return false;size_t end=start;for(;end<size&&data[end]&&end-start<64;++end){}
  if(end==start||end>=size||data[end]||end-start>63)return false;
  name.assign(reinterpret_cast<const char*>(data+start),end-start);return name.find_first_of("\r\n\t")==name.npos;
 }
 void Packet(SOCKET socket,const unsigned char* p,size_t n,bool sent){
  if(n<4)return;
  if(p[1]==0x1e){if(sent){host=false;localId=0;players.Reset();names={};ids={};humans={};}else{std::string name;if(PacketName(p,n,19,name)){players.Join(socket,name);peerNames[socket]=name;Enforce();}}return;}
  if(p[1]==0x06&&n>9){std::string name;if(PacketName(p,n,9,name)){names[p[8]]=name;if(sent&&host)for(const auto& own:ownAliases)if(!own.empty()&&name==own)localId=p[8];Rows();}return;}
  if(p[1]==0x04||p[1]==0x09){
   if(n<7)return;unsigned info=p[4]|(unsigned(p[5])<<8),count=p[6];
   if(count>24||info<1+count*9+6||6+info>n)return;
   // Only the host sends SLOTINFOJOIN. A client receiving it cannot become host.
   if(p[1]==0x04){if(6+info>=n)return;host=sent;if(!host){localId=p[6+info];players.Reset();}else if(peerNames.count(socket))names[p[6+info]]=peerNames[socket];}
   ids.fill(0);humans.fill(0);for(unsigned i=0;i<count;++i){auto slot=p+7+i*9;ids[i]=slot[0];humans[i]=slot[2]==2&&slot[3]==0;}
   Rows();return;
  }
  if(!sent&&p[1]==0x05&&notifications)Event("Join rejected by host (reason "+std::to_string(n>=8?p[4]|(unsigned(p[5])<<8):0)+")");
  if(!sent&&p[1]==0x07&&n>=9&&notifications){auto id=p[4];Event((names[id].empty()?"Player "+std::to_string(id):names[id])+" disconnected");}
 }
 void Observe(SOCKET socket,const char* data,size_t length,bool sent){
  if(!data||!length||length>65535)return;
  std::lock_guard<std::mutex> guard(mutex);if(!enabled)return;
  auto& streams=sent?outgoing:incoming;if(streams.size()>=256&&!streams.count(socket))return;
  auto& stream=streams[socket];if(stream.size()+length>131070){stream.clear();return;}
  stream.insert(stream.end(),reinterpret_cast<const unsigned char*>(data),reinterpret_cast<const unsigned char*>(data)+length);
  size_t consumed=0;
  while(stream.size()-consumed>=4){auto p=stream.data()+consumed;if(p[0]!=0xf7){stream.clear();return;}unsigned n=p[2]|(unsigned(p[3])<<8);if(n<4){stream.clear();return;}if(n>stream.size()-consumed)break;Packet(socket,p,n,sent);consumed+=n;}
  if(consumed)stream.erase(stream.begin(),stream.begin()+consumed);
 }
 void Closed(SOCKET socket){std::lock_guard<std::mutex> guard(mutex);players.Closed(socket);peerNames.erase(socket);incoming.erase(socket);outgoing.erase(socket);for(auto i=pending.begin();i!=pending.end();)if(i->second.socket==socket)i=pending.erase(i);else ++i;}
 void Track(SOCKET socket,WSABUF* buffers,DWORD count,OVERLAPPED* overlapped){
  if(!overlapped||!buffers||!count||count>64)return;std::lock_guard<std::mutex> guard(mutex);if(!enabled||pending.size()>=4096)return;pending[overlapped]={socket,std::vector<WSABUF>(buffers,buffers+count)};
 }
 void Cancel(OVERLAPPED* overlapped){std::lock_guard<std::mutex> guard(mutex);pending.erase(overlapped);}
 void Complete(OVERLAPPED* overlapped,DWORD bytes,bool success){
  Pending request{};{std::lock_guard<std::mutex> guard(mutex);auto found=pending.find(overlapped);if(found==pending.end())return;request=std::move(found->second);pending.erase(found);}
  if(!success)return;for(const auto& buffer:request.buffers){auto part=(std::min)(bytes,buffer.len);if(part)Observe(request.socket,buffer.buf,part,false);bytes-=part;if(!bytes)break;}
 }
 void Configure(const Config& c,const std::wstring& dir,const std::wstring& registryName){
  std::lock_guard<std::mutex> guard(mutex);enabled=c.mode==RunMode::Local;notifications=c.showNotiKickBan;folder=dir;newName=c.newNames;hostName=c.nameCreate;
  ownName=Utf8(!hostName.empty()?hostName:!newName.empty()?newName:registryName);
  ownAliases={Utf8(registryName),Utf8(c.oldNames),Utf8(c.newNames),Utf8(c.nameCreate)};
  if(!enabled){players.Reset();incoming.clear();outgoing.clear();pending.clear();peerNames.clear();names={};ids={};humans={};host=false;localId=0;events.clear();}
 }
 std::vector<std::string> Drain(){std::lock_guard<std::mutex> guard(mutex);std::vector<std::string> result;result.swap(events);return result;}
 bool IsHost(){std::lock_guard<std::mutex> guard(mutex);return enabled&&host;}
 std::string Kick(const std::string& target){std::lock_guard<std::mutex> guard(mutex);if(host&&!localId)return "Kick: waiting for local host identity";return enabled?players.Kick(target,Disconnect):"Kick: disabled";}
 std::string Ban(const std::string& target,const std::wstring& reason){
  std::lock_guard<std::mutex> guard(mutex);if(!enabled)return "Ban: disabled";if(host&&!localId)return "Ban: waiting for local host identity";
  return players.Ban(target,Disconnect,[&](const std::string& player){
   if(player.find_first_of("=\r\n[]")!=player.npos||reason.size()>240||reason.find_first_of(L"\r\n")!=reason.npos)return false;
   return SaveSetting(folder+L"\\TS-Banned.ini",L"Banned List",Wide(player).c_str(),reason);
  });
 }
};
inline PlayerNetwork129 playerNetwork129;
}
