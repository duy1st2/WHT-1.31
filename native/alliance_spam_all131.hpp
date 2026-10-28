#pragma once
#include "alliance_spam131.hpp"
#include "share_control131.hpp"
#include "ping_action_profile131.hpp"
namespace wht {
inline bool PingActionCode131(uintptr_t base,unsigned build){for(const auto& p:pingActionProfiles131)if(p.build==build)return ResourceTextGuard131(base,p);return false;}
inline bool PingActionPacket131(float x,float y,std::array<unsigned char,13>& out){if(!std::isfinite(x)||!std::isfinite(y)||std::abs(x)>10000.f||std::abs(y)>10000.f)return false;out={0x68};const float duration=-1.f;memcpy(out.data()+1,&x,4);memcpy(out.data()+5,&y,4);memcpy(out.data()+9,&duration,4);return true;}
inline bool SubmitPingAction131(const DropNetworkApi131& api,float x,float y){std::array<unsigned char,13> bytes;if(!api||!PingActionPacket131(x,y,bytes))return false;__try{DropBuffer131 buffer{api.table,bytes.data(),0,0,13,13};api.send(&buffer,0);return true;}__except(1){return false;}}
struct AllianceSpamAllContext131 {uintptr_t session=0;ShareControlSnapshot131 roster;};
inline bool SameSpamAllOwner131(const AllianceSpamAllContext131& a,const AllianceSpamAllContext131& b){return a.session&&a.session==b.session&&SameResourcePlayer131(a.roster.source,b.roster.source)&&a.roster.alliance&&a.roster.alliance==b.roster.alliance;}
inline bool SameSpamAllRoster131(const AllianceSpamAllContext131& a,const AllianceSpamAllContext131& b){return SameSpamAllOwner131(a,b)&&SameShareRoster131(a.roster,b.roster);}
inline bool ValidSpamAllRoster131(const AllianceSpamAllContext131& a){if(!SameSpamAllOwner131(a,a))return false;uint32_t seen=1u<<a.roster.source.physical;for(unsigned slot=0;slot<16;++slot){const auto& p=a.roster.peers[slot];if(!p.player)continue;if(p.physical!=slot||p.player==a.roster.source.player||(seen&(1u<<p.physical)))return false;seen|=1u<<p.physical;}return true;}
inline bool ReadAllianceSpamAll131(uintptr_t base,unsigned build,AllianceSpamAllContext131& out){const auto* ip=InventoryProfile(build);const auto* np=DropNetworkProfileFor131(build);if(!ip||!np||!AllianceSpamCode131(base,build)||!PingActionCode131(base,build))return false;const auto api=DropNetworkNative131(base,*np,*ip);AllianceSpamAllContext131 a;a.session=DropSession131(api);if(!a.session||!ReadShareControl131(base,build,a.roster)||DropSession131(api)!=a.session||!ValidSpamAllRoster131(a))return false;out=a;return true;}
inline bool SendSpamAllPing131(uintptr_t base,unsigned build,float x,float y){const auto* ip=InventoryProfile(build);const auto* np=DropNetworkProfileFor131(build);if(!ip||!np||!PingActionCode131(base,build))return false;const auto api=DropNetworkNative131(base,*np,*ip);return DropSession131(api)&&SubmitPingAction131(api,x,y);}
struct AllianceSpamAll131 {
 bool active=false;uint64_t due=0;uint32_t random=0x795c644a;AllianceSpamAllContext131 owner;
 void Reset(){active=false;due=0;owner={};}
 float Coordinate(){random^=random<<13;random^=random>>17;random^=random<<5;return (random&0xffffffu)*(20000.f/16777216.f)-10000.f;}
 template<class Read> bool Command(const std::wstring& args,bool allowed,Read read,std::string& report){if(args.find(L'\0')!=args.npos||args.find_first_not_of(L" \t")!=args.npos){report="Usage: #spamall";return false;}if(active){Reset();report="Alliance spam to all disabled";return true;}if(!allowed){report="Alliance spam to all: settings disabled";return false;}AllianceSpamAllContext131 a,b;if(!read(a)||!read(b)||!ValidSpamAllRoster131(a)||!ValidSpamAllRoster131(b)||!SameSpamAllRoster131(a,b)){report="Alliance spam to all: stable active player and session required";return false;}owner=b;active=true;due=0;report="Alliance spam to all enabled";return true;}
 template<class Read,class Send,class Ping> bool Tick(bool allowed,bool ready,uint64_t now,Read read,Send send,Ping ping,std::string& report){if(!allowed){Reset();return true;}if(!active||!ready||now<due)return true;due=now+1000;AllianceSpamAllContext131 a,b;auto fail=[&](){Reset();report="Alliance spam to all stopped: context changed or partial submission; no retry";return false;};if(!read(a)||!read(b)||!ValidSpamAllRoster131(a)||!ValidSpamAllRoster131(b)||!SameSpamAllOwner131(owner,a)||!SameSpamAllRoster131(a,b))return fail();
  bool pings=true;for(unsigned slot=0;slot<16;++slot){if(!b.roster.peers[slot].player)continue;for(uint32_t flags:{0xff0000ffu,0u}){AllianceSpamAllContext131 check;if(!read(check)||!SameSpamAllRoster131(b,check)||!send(slot,flags))return fail();}
   if(pings){for(unsigned i=0;i<2;++i){AllianceSpamAllContext131 check;if(!read(check)||!SameSpamAllRoster131(b,check))return fail();const float x=Coordinate(),y=Coordinate();if(!ping(x,y))return fail();}pings=false;}
  }
  AllianceSpamAllContext131 after;if(!read(after)||!SameSpamAllRoster131(b,after))return fail();return true;
 }
};
}
