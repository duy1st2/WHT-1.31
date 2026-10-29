#pragma once
#include "player_name_profile131.hpp"
#include "resource131.hpp"
namespace wht {
inline const PlayerNameProfile131* PlayerNameProfileFor131(unsigned build){for(const auto& p:playerNameProfiles131)if(p.build==build)return &p;return nullptr;}
inline bool PlayerNameCode131(uintptr_t base,const PlayerNameProfile131& p){return base&&ResourceTextGuard131(base,{p.build,0,0,0,0,0,p.code,p.count});}
// Preserve the original WHT 64-byte name limit without splitting UTF-8.
inline bool PlayerNameText131(const std::wstring& text,std::string& utf8){utf8.clear();if(text.empty()||text.size()>63)return false;for(wchar_t ch:text)if(ch<32||ch==127)return false;const int n=WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,text.data(),int(text.size()),nullptr,0,nullptr,nullptr);if(n<1||n>63)return false;utf8.resize(n);return WideCharToMultiByte(CP_UTF8,WC_ERR_INVALID_CHARS,text.data(),int(text.size()),utf8.data(),n,nullptr,nullptr)==n;}
struct PlayerNameRequest131 {bool query=false,restore=false;std::wstring text;std::string utf8;};
inline bool ParsePlayerName131(const std::wstring& args,PlayerNameRequest131& out){out={};if(args.find(L'\0')!=args.npos||args.find_first_of(L"\r\n")!=args.npos)return false;const auto first=args.find_first_not_of(L" \t");if(first==args.npos){out.query=true;return true;}out.text=args.substr(first,args.find_last_not_of(L" \t")-first+1);out.restore=out.text==L"restore"||out.text==L"default"||out.text==L"reset"||out.text==L"off";return out.restore||PlayerNameText131(out.text,out.utf8);}
// WHT #oldname controls the original name consumed by #n restore.
template<class Save,class Reload>
bool OldPlayerNameCommand131(const std::wstring& args,Config& config,Save save,Reload reload,std::string& report){
 const auto first=args.find_first_not_of(L" \t");
 if(first==args.npos){report="|cfffcd211Old Name:|r "+(config.oldNames.empty()?std::string("(none)"):Utf8(config.oldNames));return true;}
 const auto text=args.substr(first,args.find_last_not_of(L" \t\r\n")-first+1);std::string utf8;
 if(!PlayerNameText131(text,utf8)){report="Usage: #oldname [name (1..63 UTF-8 bytes)]";return false;}
 const bool primary=save(L"OLDNames",text),extended=save(L"OLDNames_Ex",text);reload();
 if(!primary||!extended||config.oldNames!=text||config.oldNamesEx!=text){report="Old Name: settings not fully saved; check #oldname before restoring";return false;}
 report="|cfffcd211Old Name:|r "+utf8;return true;
}
struct PlayerNameSource131 {uintptr_t world=0,player=0;unsigned local=24;char name[64]{};};
inline bool SamePlayerNameContext131(const PlayerNameSource131& a,const PlayerNameSource131& b){return a.world&&a.player&&a.local<24&&a.world==b.world&&a.player==b.player&&a.local==b.local;}
inline bool ReadPlayerNameRaw131(uintptr_t base,const ResourceProfile131& p,PlayerNameSource131& out){
 __try{PlayerNameSource131 v;v.world=ResourceWorld131(base,p,v.local);if(!v.world||v.local>=24)return false;const auto player=reinterpret_cast<uintptr_t(__thiscall*)(uintptr_t,unsigned)>(base+p.playerFn);v.player=player(v.world,v.local);if(!ResourceIdentity131(v.player,base,p.playerTable)||*reinterpret_cast<int*>(v.player+p.statusOffset)!=1||*reinterpret_cast<unsigned char*>(v.player+p.idOffset)>=24)return false;const auto get=reinterpret_cast<const char*(__thiscall*)(uintptr_t)>(base+p.stringFn);const auto text=get(v.player+p.nameOffset);if(!text)return false;unsigned n=0;for(;n<64&&text[n];++n)v.name[n]=text[n];if(!n||n==64)return false;unsigned local=24;if(ResourceWorld131(base,p,local)!=v.world||local!=v.local||player(v.world,local)!=v.player)return false;out=v;return true;}__except(1){return false;}
}
inline bool ReadPlayerName131(uintptr_t base,unsigned build,PlayerNameSource131& out){const auto* p=ResourceProfileFor131(build);return p&&ResourceCode131(base,*p)&&ReadPlayerNameRaw131(base,*p,out)&&MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,out.name,-1,nullptr,0)>1;}
using PlayerNameSetter131=void(__thiscall*)(uintptr_t,const char*);
inline bool InvokePlayerName131(PlayerNameSetter131 set,uintptr_t field,const char* text){if(!set||!field||!text||!*text)return false;__try{set(field,text);return true;}__except(1){return false;}}
inline bool SetPlayerName131(uintptr_t base,unsigned build,const PlayerNameSource131& before,const std::string& desired){const auto* p=PlayerNameProfileFor131(build);const auto* rp=ResourceProfileFor131(build);std::string checked;if(!p||!rp||p->nameOffset!=rp->nameOffset||!PlayerNameText131(Wide(desired),checked)||checked!=desired||!PlayerNameCode131(base,*p))return false;PlayerNameSource131 current{},after{};if(!ReadPlayerName131(base,build,current)||!SamePlayerNameContext131(before,current)||strcmp(before.name,current.name))return false;if(!InvokePlayerName131(reinterpret_cast<PlayerNameSetter131>(base+p->setString),current.player+p->nameOffset,desired.c_str()))return false;return ReadPlayerName131(base,build,after)&&SamePlayerNameContext131(current,after)&&desired==after.name;}
template<class Read,class Set,class Save,class Reload>
bool PlayerNameCommand131(const std::wstring& command,const std::wstring& args,Config& config,Read read,Set set,Save save,Reload reload,std::string& report){
 PlayerNameRequest131 request;if(!ParsePlayerName131(args,request)){report="Usage: #n [name (1..63 UTF-8 bytes)|restore]";return false;}
 PlayerNameSource131 before{};if(!read(before)){report="Player Name: local player unavailable (active match required)";return false;}
 const std::string prefix=command==L"#name"?"[64] ":"";
 if(request.query){report=prefix+"Player Name: "+before.name;return true;}
 if(request.restore){request.text=config.oldNames;if(!PlayerNameText131(request.text,request.utf8)){report="[WHT] No valid original name recorded to restore";return false;}}
 if(config.oldNames.empty()&&!request.restore){const auto old=Wide(before.name);if(!save(L"OLDNames",old)||!save(L"OLDNames_Ex",old)){reload();report="Player Name: original name could not be saved; no rename attempted";return false;}reload();}
 if(request.utf8!=before.name&&!set(before,request.utf8)){report="Player Name: native rename could not be confirmed; new name setting unchanged";return false;}
 PlayerNameSource131 after{};if(!read(after)||!SamePlayerNameContext131(before,after)||request.utf8!=after.name){report="Player Name: player or name changed during rename; new name setting unchanged";return false;}
 const auto saved=request.restore?std::wstring():request.text;
 const bool first=save(L"NewNames",saved),second=save(L"NewNames_Ex",saved);reload();
 report=prefix+"Set Name: "+before.name+" -> "+request.utf8;
 if(!first||!second){report+=" (name changed, settings not fully saved)";return false;}return true;
}
// As in WHT, keep an explicitly configured name applied while in a match.
// A failed native attempt is quarantined until the world or configured name changes.
struct PlayerNamePolicy131 {
 uint64_t next=0;uintptr_t world=0,player=0;std::wstring wanted;bool blocked=false;
 void Reset(){next=0;world=player=0;wanted.clear();blocked=false;}
 template<class Read,class Set,class Save,class Reload>
 void Update(uint64_t now,bool allowed,Config& config,Read read,Set set,Save save,Reload reload,std::string& report){
  if(!allowed||config.newNames.empty()){Reset();return;}if(now<next)return;next=now+1000;
  std::string desired;if(!PlayerNameText131(config.newNames,desired))return;PlayerNameSource131 source{};if(!read(source))return;
  if(world!=source.world||player!=source.player||wanted!=config.newNames){world=source.world;player=source.player;wanted=config.newNames;blocked=false;}
  if(blocked||desired==source.name)return;
  if(config.oldNames.empty()){const auto old=Wide(source.name);if(!save(L"OLDNames",old)||!save(L"OLDNames_Ex",old)){reload();blocked=true;report="Player Name: original name could not be saved; automatic rename paused";return;}reload();}
  if(!set(source,desired)){blocked=true;report="Player Name: native rename could not be confirmed; automatic rename paused";}
 }
};
}
