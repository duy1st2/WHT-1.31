#pragma once
#include "network_ping.hpp"
#include <cmath>
#include <cstdio>
namespace wht {
struct NativePingProfile {uintptr_t getter=0,data=0;bool wide=false;};
inline NativePingProfile PingProfile129(){return {0x6b2b20,0xdb4a0c,false};}
inline NativePingProfile PingProfile131(unsigned build){
#ifdef _WIN64
 if(build==12164)return {0x17a8550,0x22b50e0,true};if(build==12173)return {0x17e4140,0x22e20e0,true};
#else
 if(build==12164)return {0x14e8e70,0x1c62058,false};if(build==12173)return {0x14a8af0,0x1c1a058,false};
#endif
 return {};
}
// Read only the float used by the stock Ping formatter, after validating its entire getter.
inline bool ReadNativePing(uintptr_t base,const NativePingProfile& p,float& ms){
 if(!base||!p.getter||!p.data||p.wide!=(sizeof(void*)==8))return false;
 __try {const auto code=reinterpret_cast<const unsigned char*>(base+p.getter);
  if(p.wide){if(code[0]!=0xf3||code[1]!=0x0f||code[2]!=0x10||code[3]!=0x05||code[8]!=0xc3||base+p.getter+8+*reinterpret_cast<const int32_t*>(code+4)!=base+p.data)return false;}
  else if(code[0]!=0xd9||code[1]!=0x05||code[6]!=0xc3||*reinterpret_cast<const uint32_t*>(code+2)!=base+p.data)return false;
  const float value=*reinterpret_cast<const float*>(base+p.data);if(!std::isfinite(value)||value<=0||value>60000)return false;ms=value;return true;
 }__except(EXCEPTION_EXECUTE_HANDLER){return false;}
}
inline std::string NativePingText(float ms){char text[96]{};std::snprintf(text,sizeof(text),"Ping: %.2f ms (game native)",double(ms));return text;}
inline std::wstring PingStatus(uintptr_t base,const NativePingProfile& profile){float ms=0;if(ReadNativePing(base,profile,ms)){auto text=NativePingText(ms);return std::wstring(text.begin(),text.end());}return network_ping::Status();}
// Legacy HUD: use a fresh matched echo only; never substitute a loopback TCP RTT.
inline std::string EchoPingHud(uint64_t now){std::lock_guard<std::mutex> lock(network_ping::mutex);uint64_t maximum=0;bool found=false,local=false;
 for(const auto& peer:network_ping::connections){if(!peer.sampled||now<peer.measured||now-peer.measured>10000)continue;sockaddr_in address{};int size=sizeof(address);if(getpeername(peer.socket,reinterpret_cast<sockaddr*>(&address),&size)||address.sin_family!=AF_INET)continue;maximum=(std::max)(maximum,uint64_t(peer.rtt));found=true;local|=network_ping::Loopback(address);}
 if(!found){uint64_t tcp=0;for(const auto& peer:network_ping::connections){if(!peer.recognized||now<peer.lastSend||now-peer.lastSend>30000)continue;sockaddr_in address{};int size=sizeof(address);if(getpeername(peer.socket,reinterpret_cast<sockaddr*>(&address),&size)||address.sin_family!=AF_INET||network_ping::Loopback(address))continue;TCP_INFO_v0 info{};DWORD version=0,bytes=0;if(WSAIoctl(peer.socket,SIO_TCP_INFO,&version,sizeof(version),&info,sizeof(info),&bytes,nullptr,nullptr)==0&&bytes>=sizeof(info)&&info.State==TCPSTATE_ESTABLISHED&&info.RttUs>0)tcp=(std::max)(tcp,(uint64_t(info.RttUs)+500)/1000);}if(tcp)return "Ping: "+std::to_string(tcp)+" ms (TCP max)";return "Ping: -- ms";}char text[112]{};std::snprintf(text,sizeof(text),"Ping: %s (echo max%s)",maximum?std::to_string(maximum).append(" ms").c_str():"<1 ms",local?", includes local":"");return text;
}
}
