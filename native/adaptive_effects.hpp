#pragma once
#include <cmath>
#include <cstdint>

namespace wht {
// Rendering-only particle emission budget. Timers use elapsed time, not ticks.
struct AdaptiveEffectsPolicy {
    int level=0; // 0: original, 1: 50%, 2: 25%, 3: no new particles
    uint64_t lowAt=0;
    bool low=false,critical=false;uint64_t criticalAt=0;
    bool recovering=false;uint64_t recoveryAt=0;
    void Suspend(){low=critical=recovering=false;}
    int Update(float fps,uint64_t now) {
        if(!std::isfinite(fps)||fps<=0){Suspend();return level;}
        if(fps<15){
            if(!critical){critical=true;criticalAt=now;}
            if(now-criticalAt>=500){level=3;low=false;recovering=false;return level;}
        }else critical=false;
        if(fps<50){
            recovering=false;
            if(!low){low=true;lowAt=now;}
            if(now-lowAt>=2000&&level<2){++level;lowAt=now;}
        }else low=false;
        // Separate enter/recover thresholds and slow one-step recovery avoid
        // oscillating emission on every wave or counting a paused game as recovery.
        if(fps>=58&&level){
            if(!recovering){recovering=true;recoveryAt=now;}
            if(now-recoveryAt>=8000){--level;recoveryAt=now;}
        }else recovering=false;
        return level;
    }
};

struct AdaptiveEffectsState {
    AdaptiveEffectsPolicy policy;
    uintptr_t session=0;
    float original=1, applied=1;
    bool owned=false, sampled=false;
    uint64_t nextSample=0;
    bool Due(uint64_t now){if(sampled&&now<nextSample)return false;sampled=true;nextSample=now+250;return true;}
    void Suspend(){policy.Suspend();}
    template<class Setter> bool Restore(float current,Setter set){
        bool ok=true;
        if(owned&&current==applied)ok=set(original);
        if(ok){owned=false;policy={};session=0;sampled=false;}
        return ok;
    }
    template<class Setter> bool Update(uintptr_t root,bool enabled,float fps,float current,uint64_t now,Setter set){
        if(!std::isfinite(current)||current<0||current>1)return false;
        if(!enabled||!root)return Restore(current,set);
        if(session!=root){
            // Re-read the restored native scale on the next sample.
            if(owned)return Restore(current,set);
            policy={};session=root;original=current;
        }
        // Respect a video-setting change made outside this controller.
        if(owned&&current!=applied){owned=false;policy={};original=current;}
        if(!owned&&policy.level==0)original=current;
        const int before=policy.level;
        const int level=policy.Update(fps,now);
        const float target=level==3?0.f:level==2?original*.25f:level==1?original*.5f:original;
        if(target!=current&&!set(target)){policy.level=before;policy.Suspend();return false;}
        applied=target;owned=level!=0&&target!=original;
        return true;
    }
};
}
