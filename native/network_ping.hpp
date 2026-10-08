#pragma once
#include <winsock2.h>
#include <mstcpip.h>
#include <mutex>
#include <vector>
#include <string>
#include <algorithm>
#include <cstdint>

namespace wht::network_ping {
struct Echo {uint32_t token;unsigned char response;ULONGLONG sent;};
struct Connection {
    SOCKET socket;ULONGLONG lastSend;
    std::vector<unsigned char> outgoing,incoming;
    std::vector<Echo> pending;
    ULONGLONG measured=0,rtt=0;bool sampled=false,recognized=false;
};
inline std::mutex mutex;
inline std::vector<Connection> connections;
inline void Reset(){std::lock_guard<std::mutex> lock(mutex);connections.clear();}
inline void Closed(SOCKET socket){
    std::lock_guard<std::mutex> lock(mutex);
    connections.erase(std::remove_if(connections.begin(),connections.end(),[&](const auto& c){return c.socket==socket;}),connections.end());
}
inline uint32_t Token(const unsigned char* p){return uint32_t(p[0])|(uint32_t(p[1])<<8)|(uint32_t(p[2])<<16)|(uint32_t(p[3])<<24);}
inline void Packet(Connection& c,const unsigned char* data,unsigned length,bool outgoing,ULONGLONG now){
    c.recognized=true;
    const auto type=data[1];
    // PeerPing carries cookie, peer set and game ticks; its pong echoes only cookie.
    if(length!=(type==0x35?16u:8u))return;
    const auto token=Token(data+4);
    if(outgoing&&(type==0x01||type==0x35)){
        c.pending.erase(std::remove_if(c.pending.begin(),c.pending.end(),[&](const auto& e){return now>=e.sent&&now-e.sent>30000;}),c.pending.end());
        const auto response=static_cast<unsigned char>(type==0x01?0x46:0x36);
        // Duplicate cookies cannot establish an unambiguous start time.
        for(const auto& e:c.pending)if(e.token==token&&e.response==response)return;
        if(c.pending.size()<16)c.pending.push_back({token,response,now});
    }else if(!outgoing&&(type==0x46||type==0x36)){
        for(auto it=c.pending.begin();it!=c.pending.end();++it){
            if(it->token==token&&it->response==type){
                if(now>=it->sent&&now-it->sent<=30000){c.rtt=now-it->sent;c.measured=now;c.sampled=true;}
                c.pending.erase(it);break;
            }
        }
    }
}
// TCP is a byte stream: reconstruct fragmented/coalesced W3GS frames separately
// in each direction. Observe only bytes actually sent/received, never MSG_PEEK.
inline void ObserveBytes(SOCKET socket,const char* data,int size,bool outgoing,ULONGLONG now){
    if(!data||size<=0)return;
    std::lock_guard<std::mutex> lock(mutex);
    connections.erase(std::remove_if(connections.begin(),connections.end(),[&](const auto& c){return now>=c.lastSend&&now-c.lastSend>30000;}),connections.end());
    auto it=std::find_if(connections.begin(),connections.end(),[&](const auto& c){return c.socket==socket;});
    if(it==connections.end()){
        if(static_cast<unsigned char>(data[0])!=0xf7||connections.size()>=32)return;
        connections.push_back({socket,now});it=connections.end()-1;
    }
    auto& c=*it;c.lastSend=std::max(c.lastSend,now);
    auto& stream=outgoing?c.outgoing:c.incoming;
    // Consume incrementally so a large batch of valid frames is not rejected.
    for(int pos=0;pos<size;){
        if(stream.empty()&&static_cast<unsigned char>(data[pos])!=0xf7)return;
        const auto take=std::min<int>(size-pos,static_cast<int>(65535-stream.size()));
        if(take<=0){stream.clear();return;}
        stream.insert(stream.end(),data+pos,data+pos+take);pos+=take;
        size_t consumed=0;
        while(stream.size()-consumed>=4){
            const auto* frame=stream.data()+consumed;
            const unsigned length=unsigned(frame[2])|(unsigned(frame[3])<<8);
            if(frame[0]!=0xf7||length<4){stream.clear();return;}
            if(stream.size()-consumed<length)break;
            Packet(c,frame,length,outgoing,now);consumed+=length;
        }
        if(consumed)stream.erase(stream.begin(),stream.begin()+consumed);
    }
}
inline void Observe(SOCKET socket,const char* data,int size,int sent){
    if(sent>0)ObserveBytes(socket,data,std::min(size,sent),true,GetTickCount64());
}
inline void Received(SOCKET socket,const char* data,int size){ObserveBytes(socket,data,size,false,GetTickCount64());}
inline bool Loopback(const sockaddr_in& peer){return reinterpret_cast<const unsigned char*>(&peer.sin_addr)[0]==127;}
inline std::wstring Milliseconds(ULONGLONG ms){return ms?std::to_wstring(ms)+L" ms":L"<1 ms";}
inline std::wstring PeerName(const sockaddr_in& peer){
    const auto* ip=reinterpret_cast<const unsigned char*>(&peer.sin_addr);
    return std::to_wstring(ip[0])+L"."+std::to_wstring(ip[1])+L"."+std::to_wstring(ip[2])+L"."+std::to_wstring(ip[3]);
}
inline std::wstring Status(){
    const int savedError=WSAGetLastError();std::wstring result;bool proxy=false;
    const auto now=GetTickCount64();
    {
        std::lock_guard<std::mutex> lock(mutex);
        for(auto it=connections.begin();it!=connections.end();){
            if(!it->recognized){++it;continue;}
            sockaddr_in peer{};int size=sizeof(peer);
            if(now-it->lastSend>30000||getpeername(it->socket,reinterpret_cast<sockaddr*>(&peer),&size)!=0){it=connections.erase(it);continue;}
            if(peer.sin_family!=AF_INET){++it;continue;}
            const bool local=Loopback(peer);proxy|=local;std::wstring line;
            if(it->sampled&&now-it->measured<=45000){
                line=L"Ping "+PeerName(peer)+L": "+Milliseconds(it->rtt)+L" (Warcraft echo RTT"+
                    (local?std::wstring(L"; local proxy, remote endpoint unverified"):std::wstring())+L")";
            }else if(!local){
                TCP_INFO_v0 info{};DWORD version=0,bytes=0;
                if(WSAIoctl(it->socket,SIO_TCP_INFO,&version,sizeof(version),&info,sizeof(info),&bytes,nullptr,nullptr)==0&&
                   bytes>=sizeof(info)&&info.State==TCPSTATE_ESTABLISHED&&info.RttUs>0){
                    line=L"Ping "+PeerName(peer)+L": "+Milliseconds((static_cast<ULONGLONG>(info.RttUs)+500)/1000)+L" (TCP RTT fallback; waiting for Warcraft echo)";
                }
            }
            if(!line.empty()){if(!result.empty())result+=L"\n";result+=line;}
            ++it;
        }
    }
    WSASetLastError(savedError);
    if(!result.empty())return result;
    if(proxy)return L"Ping: local proxy detected; no remote RTT confirmed. Waiting for Warcraft echo; TCP RTT to 127.x.x.x is not host ping.";
    return L"Ping: no active network measurement (offline, waiting for echo, or unsupported connection).";
}
}
