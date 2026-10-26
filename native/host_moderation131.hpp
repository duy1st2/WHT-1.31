#pragma once
#include "common.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <unordered_map>
namespace wht {
// Independent transport-only implementation. No 1.29 game function or layout.
struct ModerationCommand131 {std::wstring target,reason;};
inline bool ParseModeration131(const std::wstring& command,const std::wstring& args,ModerationCommand131& out){
 if(args.find_first_of(L"\r\n")!=args.npos||args.find(L'\0')!=args.npos)return false;
 std::wistringstream in(args);ModerationCommand131 value;std::wstring extra;
 if(command==L"#openbl"){if(in>>extra)return false;out=value;return true;}
 if(command!=L"#kick"&&command!=L"#ban")return false;
 in>>std::ws;if(in.eof())return false;
 if(in.peek()==L'"'){if(!(in>>std::quoted(value.target)))return false;}
 else if(command==L"#ban"){if(!(in>>value.target))return false;}
 else std::getline(in,value.target);
 if(command==L"#ban"){std::getline(in,value.reason);auto first=value.reason.find_first_not_of(L" \t");value.reason=first==value.reason.npos?L"No reason":value.reason.substr(first);}
 else if(in>>extra)return false;
 auto last=value.target.find_last_not_of(L" \t");if(last==value.target.npos)return false;value.target.resize(last+1);
 if(value.target.size()>63||value.reason.size()>240||(command==L"#ban"&&value.target.find_first_of(L"=\r\n[]")!=value.target.npos))return false;
 out=std::move(value);return true;
}
struct HostModeration131 {
 struct Peer {uintptr_t socket=0;std::string name;unsigned id=0;bool pending=false;unsigned banAttempts=0;};
 struct Row {unsigned id=0;bool human=false;};
 std::unordered_map<uintptr_t,Peer> peers;
 std::array<std::string,256> names{};std::array<bool,256> announced{};std::array<Row,24> rows{};
 std::string ownName;unsigned ownId=0;bool host=false,client=false;
 void Reset(){peers.clear();names={};announced={};rows={};ownId=0;host=client=false;}
 void Identity(const std::string& name){
  if(name.empty()||name==ownName)return;
  // First trusted identity may become available only when the game world exists.
  // Preserve the captured handshake; an actual identity change still clears it.
  if(!ownName.empty())Reset();ownName=name;
  if(host&&!client){unsigned candidate=0;for(unsigned i=1;i<names.size();++i)if(names[i]==ownName){if(candidate){candidate=0;break;}candidate=i;}ownId=candidate;}
 }
 // A host's own human row is the unique row without an accepted peer connection.
 // Require its PLAYERINFO to have been sent by this process; never infer from received names.
 void ResolveHostIdentity(){if(!host||client||!ownName.empty())return;unsigned candidate=0;
  for(const auto& row:rows)if(row.human){bool remote=false;for(const auto& entry:peers)if(entry.second.id==row.id)remote=true;if(!remote){if(candidate)return;candidate=row.id;}}
  if(candidate&&announced[candidate]&&!names[candidate].empty()){ownId=candidate;ownName=names[candidate];}
 }
 void Closed(uintptr_t socket){peers.erase(socket);}
 static bool Name(const unsigned char* p,size_t n,size_t offset,std::string& name){
  if(offset>=n)return false;size_t end=offset;while(end<n&&p[end]&&end-offset<=63)++end;
  if(end==offset||end>=n||end-offset>63||p[end])return false;
  for(size_t i=offset;i<end;++i)if(p[i]<32||p[i]==127)return false;
  if(!MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,reinterpret_cast<const char*>(p+offset),int(end-offset),nullptr,0))return false;
  name.assign(reinterpret_cast<const char*>(p+offset),end-offset);return true;
 }
 // Runtime profile: full W3GS framing, incoming REQJOIN, outgoing SLOTINFOJOIN,
 // matching recipient ID/name and local PLAYERINFO. Any client join revokes host.
 bool Packet(uintptr_t socket,const unsigned char* p,size_t n,bool sent){
  if(n<4||p[0]!=0xf7||size_t(p[2]|unsigned(p[3])<<8)!=n)return false;
  if(p[1]==0x1e){
   std::string name;if(!Name(p,n,19,name))return false;
   if(sent){Reset();client=true;}else if(!client&&peers.size()<256){peers[socket]={socket,name,0,false};}
   return true;
  }
  if(p[1]==6){std::string name;if(n<11||!p[8]||!Name(p,n,9,name))return false;names[p[8]]=name;if(sent)announced[p[8]]=true;
   if(sent&&!client&&!ownName.empty()&&name==ownName)ownId=p[8];if(sent)ResolveHostIdentity();return true;}
  if(p[1]==4||p[1]==9){
   if(n<7)return false;const unsigned size=p[4]|unsigned(p[5])<<8,count=p[6];
   if(!count||count>24||size!=1+9*count+6||6u+size>n)return false;
   std::array<Row,24> next{};std::array<bool,256> used{};
   for(unsigned i=0;i<count;++i){const auto row=p+7+9*i;const unsigned id=row[0];const bool human=row[2]==2&&row[3]==0;
    if(human&&(!id||used[id]))return false;if(human)used[id]=true;next[i]={id,human};}
   if(p[1]==4){if(n<6u+size+1||!p[6+size])return false;
    if(!sent){Reset();client=true;return true;}
    auto peer=peers.find(socket);if(client||peer==peers.end())return false;
    const unsigned id=p[6+size];if(id==ownId)return false;for(const auto& entry:peers)if(entry.first!=socket&&entry.second.id==id)return false;if(peer->second.id&&peer->second.id!=id)return false;
    if(!names[id].empty()&&names[id]!=peer->second.name)return false;
    peer->second.id=id;names[id]=peer->second.name;host=true;
   }else if(!sent||!host||client)return true;
   rows=next;ResolveHostIdentity();return true;
  }
  if(p[1]==7){if(n<9)return false;const unsigned id=p[4];for(auto i=peers.begin();i!=peers.end();)if(i->second.id==id)i=peers.erase(i);else ++i;for(auto& row:rows)if(row.id==id)row={};names[id].clear();}
  return true;
 }
 bool Ready()const {if(!host||client||!ownId||ownName.empty()||names[ownId]!=ownName)return false;for(const auto& row:rows)if(row.human&&row.id==ownId)return true;return false;}
 template<class Disconnect,class Save> std::string Act(const std::string& target,bool ban,Disconnect disconnect,Save save){
  if(!Ready())return "[WHT] Kick: verified host/player connections are not available; no action taken.";
  unsigned slot=0;bool numeric=!target.empty();for(unsigned char c:target){if(c<'0'||c>'9'){numeric=false;break;}if(slot>24)return "[WHT] Kick: slot must be 1-24.";slot=slot*10+c-'0';}
  const Row* chosen=nullptr;
  if(numeric){if(!slot||slot>24)return "[WHT] Kick: slot must be 1-24.";chosen=&rows[slot-1];}
  else for(const auto& row:rows)if(row.human&&names[row.id]==target){if(chosen)return "[WHT] Kick: ambiguous player name; use slot number.";chosen=&row;}
  if(!chosen||!chosen->human||names[chosen->id].empty())return "[WHT] Kick: no human player in that slot.";
  if(chosen->id==ownId||names[chosen->id]==ownName)return "[WHT] Kick: cannot kick yourself.";
  Peer* selected=nullptr;for(auto& entry:peers){auto& peer=entry.second;if(peer.id==chosen->id&&peer.name==names[chosen->id]){if(selected)return "[WHT] Kick: ambiguous connection; no action taken.";selected=&peer;}}
  if(!selected)return "[WHT] Kick: player connection not recorded; no action taken.";
  if(selected->pending)return "[WHT] Kick: disconnect already requested.";
  if(ban&&!save(selected->name))return "[WHT] Could not save ban list; no disconnect requested.";
  const bool result=disconnect(selected->socket);if(result)selected->pending=true;
  return (ban?"[WHT] Ban saved for "+selected->name+". ":std::string("[WHT] "))+(result?"Disconnect requested: "+selected->name:"Kick: socket shutdown failed; no disconnect confirmed.");
 }
 template<class Banned,class Disconnect> void Enforce(Banned banned,Disconnect disconnect){
  if(!Ready())return;for(auto& entry:peers){auto& peer=entry.second;if(!peer.id||peer.id==ownId||peer.name==ownName||peer.pending||peer.banAttempts>=2||!banned(peer.name))continue;
   bool matched=false;for(const auto& row:rows)if(row.human&&row.id==peer.id&&names[row.id]==peer.name)matched=true;
   if(matched){++peer.banAttempts;if(disconnect(peer.socket))peer.pending=true;}}
 }
};
}
