#pragma once
#include "resource_transfer_network131.hpp"
#include "alliance_spam_profile131.hpp"
namespace wht {
inline bool AllianceSpamCode131(uintptr_t base,unsigned build){for(const auto& p:allianceSpamProfiles131)if(p.build==build)return ResourceTextGuard131(base,p);return false;}
inline std::array<unsigned char,6> AllianceSpamPacket131(unsigned slot,uint32_t flags){std::array<unsigned char,6> b{{0x50,static_cast<unsigned char>(slot)}};for(unsigned i=0;i<4;++i)b[i+2]=static_cast<unsigned char>(flags>>(8*i));return b;}
inline bool SubmitAllianceSpam131(const DropNetworkApi131& api,unsigned slot,uint32_t flags){if(!api||slot>=16||(flags!=0xff0000ffu&&flags!=0))return false;const auto bytes=AllianceSpamPacket131(slot,flags);__try{DropBuffer131 buffer{api.table,bytes.data(),0,0,6,6};api.send(&buffer,0);return true;}__except(1){return false;}}
inline bool ParseAllianceSpam131(const std::wstring& args,int& slot){if(args.find(L'\0')!=args.npos||args.find_first_of(L"\r\n")!=args.npos)return false;std::wistringstream in(args);in.imbue(std::locale::classic());std::wstring token,extra;slot=-1;if(!(in>>token))return true;if(token.empty()||token.size()>2||token.find_first_not_of(L"0123456789")!=token.npos||in>>extra)return false;const unsigned n=unsigned(std::stoul(token));if(n<1||n>16)return false;slot=int(n)-1;return true;}
struct AllianceSpamContext131 {uintptr_t session=0;ResourceTransferSource131 players;};
inline bool SameAllianceSpam131(const AllianceSpamContext131& a,const AllianceSpamContext131& b){return a.session&&a.session==b.session&&SameResourceTransfer131(a.players,b.players);}
// One source-compatible pair per second. Never replay missed intervals or retry
// an uncertain native submission. No minimap actions are used by single #spam.
struct AllianceSpam131 {
 int slot=-1;uint64_t due=0;AllianceSpamContext131 owner;
 void Reset(){slot=-1;due=0;owner={};}
 template<class Read> bool Command(const std::wstring& args,bool allowed,Read read,std::string& report){int target=-1;if(!ParseAllianceSpam131(args,target)){report="Usage: #spam <1..16>; #spam to stop";return false;}if(target<0){Reset();report="Alliance spam disabled";return true;}if(!allowed){Reset();report="Alliance spam: settings disabled";return false;}AllianceSpamContext131 a,b;if(!read(unsigned(target),a)||!read(unsigned(target),b)||!SameAllianceSpam131(a,b)){report="Alliance spam: stable active remote player and session required";return false;}slot=target;owner=b;due=0;report="Alliance spam enabled for Player "+std::to_string(target+1);return true;}
 template<class Read,class Send> bool Tick(bool allowed,bool ready,uint64_t now,Read read,Send send,std::string& report){if(!allowed){Reset();return true;}if(slot<0||!ready||now<due)return true;due=now+1000;AllianceSpamContext131 a,b;const auto target=unsigned(slot);if(!read(target,a)||!SameAllianceSpam131(owner,a)||!read(target,b)||!SameAllianceSpam131(a,b)){Reset();report="Alliance spam stopped: player or session changed";return false;}
  for(const uint32_t flags:{0xff0000ffu,0u}){AllianceSpamContext131 check;if(!read(target,check)||!SameAllianceSpam131(b,check)||!send(target,flags)){Reset();report="Alliance spam stopped: native pair may be incomplete; no retry";return false;}}
  AllianceSpamContext131 after;if(!read(target,after)||!SameAllianceSpam131(b,after)){Reset();report="Alliance spam stopped: session changed after submission";return false;}return true;
 }
};
inline bool ReadAllianceSpam131(uintptr_t base,unsigned build,unsigned slot,AllianceSpamContext131& out){const auto* ip=InventoryProfile(build);const auto* np=DropNetworkProfileFor131(build);if(slot>=16||!ip||!np||!AllianceSpamCode131(base,build))return false;const auto api=DropNetworkNative131(base,*np,*ip);AllianceSpamContext131 a;a.session=DropSession131(api);if(!a.session||!ReadResourceTransfer131(base,build,slot,a.players)||a.players.target.physical!=slot||DropSession131(api)!=a.session)return false;out=a;return true;}
inline bool SendAllianceSpam131(uintptr_t base,unsigned build,unsigned slot,uint32_t flags){const auto* ip=InventoryProfile(build);const auto* np=DropNetworkProfileFor131(build);if(!ip||!np||!AllianceSpamCode131(base,build))return false;const auto api=DropNetworkNative131(base,*np,*ip);return DropSession131(api)&&SubmitAllianceSpam131(api,slot,flags);}
}
