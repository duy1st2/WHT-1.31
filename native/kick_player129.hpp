#pragma once
#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace wht {
class KickPlayers129 {
    struct Peer {uintptr_t socket;std::string name;bool pending=false;};
    struct Slot {std::string name;bool human=false,local=false;};
    std::vector<Peer> peers;
    std::array<Slot,24> slots{};
    bool host=false;
public:
    void Reset(){peers.clear();slots={};host=false;}
    void Row(unsigned index,const std::string& name,bool human,bool local,bool isHost){
        if(index>=slots.size())return;
        host=isHost;slots[index]={name,human,local};
    }
    void Join(uintptr_t socket,const std::string& name){
        if(socket==static_cast<uintptr_t>(-1)||name.empty()||name.size()>63)return;
        for(auto& p:peers)if(p.socket==socket){if(p.name!=name)p={socket,name,false};return;}
        if(peers.size()<256)peers.push_back({socket,name,false});
    }
    void Closed(uintptr_t socket){
        for(auto it=peers.begin();it!=peers.end();){if(it->socket==socket)it=peers.erase(it);else ++it;}
    }
    // Caller holds the same lock used by closesocket, through shutdown.
    template<class Disconnect> std::string Kick(const std::string& target,Disconnect disconnect){
        return Act(target,disconnect,[](const std::string&){return true;});
    }
    template<class Disconnect,class Save> std::string Ban(const std::string& target,Disconnect disconnect,Save save){
        std::string saved;
        auto result=Act(target,disconnect,[&](const std::string& name){if(!save(name))return false;saved=name;return true;});
        return saved.empty()?"[WHT] Ban not saved. "+result:"[WHT] Ban saved for "+saved+". "+result;
    }
    template<class IsBanned,class Disconnect> void EnforceBans(IsBanned banned,Disconnect disconnect){
        if(!host)return;
        for(auto& p:peers){
            bool self=false;for(const auto& s:slots)if(s.local&&s.name==p.name)self=true;
            if(!self&&!p.pending&&banned(p.name)&&disconnect(p.socket))p.pending=true;
        }
    }
private:
    template<class Disconnect,class Save> std::string Act(const std::string& target,Disconnect disconnect,Save save){
        if(!host)return "[WHT] Kick: only the host can kick players.";
        if(target.empty())return "[WHT] Usage: #kick <1-24 or exact player name>";
        unsigned number=0;bool numeric=true;
        for(unsigned char c:target){if(c<'0'||c>'9'){numeric=false;break;}if(number>24)return "[WHT] Kick: slot must be 1-24.";number=number*10+c-'0';}
        const Slot* chosen=nullptr;
        if(numeric){if(!number||number>24)return "[WHT] Kick: slot must be 1-24.";chosen=&slots[number-1];}
        else for(const auto& s:slots)if(s.human&&s.name==target){if(chosen)return "[WHT] Kick: ambiguous player name; use slot number.";chosen=&s;}
        if(!chosen||!chosen->human||chosen->name.empty())return "[WHT] Kick: no human player in that slot.";
        if(chosen->local)return "[WHT] Kick: cannot kick yourself.";
        if(!save(chosen->name))return "[WHT] Could not save ban list; no disconnect requested.";
        Peer* peer=nullptr;
        for(auto& p:peers)if(p.name==chosen->name){if(peer)return "[WHT] Kick: ambiguous connection; wait for lobby refresh.";peer=&p;}
        if(!peer)return "[WHT] Kick: player connection not recorded; no action taken.";
        if(peer->pending)return "[WHT] Kick: disconnect already requested.";
        if(!disconnect(peer->socket))return "[WHT] Kick: socket shutdown failed; no success confirmed.";
        peer->pending=true;
        return "[WHT] Disconnect requested: "+chosen->name;
    }
};
}
