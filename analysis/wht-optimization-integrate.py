from pathlib import Path
r=Path('WHT-1.31/native')
p=r/'state_bars131.hpp';s=p.read_text(encoding='utf-8-sig').replace('#include "world_present131.hpp"','#include "world_present131.hpp"\n#include "overhead_sampling131.hpp"')
s=s.replace('HpPlayerColor131 hpColors;HpDimensions131 hpDimensions;','OverheadSampling131 sampling;\n HpPlayerColor131 hpColors;HpDimensions131 hpDimensions;').replace('void Reset(){ForgetStatPaint131();','void Reset(){sampling.Reset();ForgetStatPaint131();')
s=s.replace('const Config& config,bool allowed,const NativeFrameApi& raw){\n  std::unordered_set<uintptr_t> frames;', 'const Config& config,bool allowed,const NativeFrameApi& raw,uintptr_t selected=0){\n  const auto now=GetTickCount64();sampling.Configure(reinterpret_cast<uintptr_t>(root),selected,config,allowed);if(!sampling.Full(now))return observedNative;\n  std::unordered_set<uintptr_t> frames;')
s=s.replace('for(const auto bar:frames){uintptr_t unit=0;bool visible=false,hero=false;','unsigned count=0;for(const auto bar:frames){uintptr_t unit=0;bool visible=false,hero=false;').replace('if(!SampleStateBar131(bar,base,s,r,unit,visible,hero))continue;','if(!SampleStateBar131(bar,base,s,r,unit,visible,hero))continue;++count;')
s=s.replace('  return observedNative;\n }\n unsigned WriteBar','  sampling.Load(count);return observedNative;\n }\n unsigned WriteBar')
needle='   hpColors.Update(base,p.build,bar,unit,allowed,GetTickCount64(),config);';assert s.count(needle)==1
sample='''   const bool eligibleNow=OverheadEligible131(allowed,config.hideInvulnerableBars,OverheadUnitFlags131(unit));
   auto cached=entries.find(bar);
   if(cached!=entries.end()&&cached->second.host&&!WorldChild131(cached->second.host,bar,base,p.simpleVtable))sampling.Invalidate(unit);
   uintptr_t bound=0;uint64_t identity=0;
   if(StatPaintPresentationKey131(bar,base+r.unit,bound,identity)&&bound==unit&&!sampling.Unit(unit,identity,bar,hero,GetTickCount64(),config,allowed)){
    if(cached!=entries.end()&&cached->second.host){if(!eligibleNow)HideWorldHost131(cached->second.host);else if(!cached->second.text[0].empty()||!cached->second.text[1].empty()||!cached->second.levelText.empty())MaintainWorldHost131(cached->second.host,bar);}
    return observed;
   }
'''
s=s.replace(needle,sample+needle);p.write_text(s,encoding='utf-8')
p=r/'gameplay64.hpp';s=p.read_text(encoding='utf-8-sig').replace('#include "world_present131.hpp"','#include "world_present131.hpp"\n#include "overhead_sampling131.hpp"')
s=s.replace(' HpPlayerColor131 hpColors;HpDimensions131 hpDimensions;',' OverheadSampling131 sampling;ButtonUpdateCadence regenCadence,cooldownCadence;\n HpPlayerColor131 hpColors;HpDimensions131 hpDimensions;').replace('void Reset(){ForgetStatPaint131();','void Reset(){sampling.Reset();regenCadence={};cooldownCadence={};ForgetStatPaint131();')
s=s.replace('  auto selected=Selected64(ui,base,p);','  auto selected=Selected64(ui,base,p);const auto now=GetTickCount64();\n  if(regenCadence.Due(now,ui,selected,nullptr,(allowed&&c.showRegenHP?1u:0u)|(allowed&&c.showRegenMana?2u:0u))){')
needle='  for(unsigned slot=0;slot<18;++slot){auto& e=cooldown[slot];auto button=Button64(ui,base,p,slot);';assert s.count(needle)==1
s=s.replace(needle,'  }\n  if(cooldownCadence.Due(now,ui,selected,&p,allowed&&c.showCooldown?1u:0u)){\n  for(unsigned slot=0;slot<18;++slot){auto& e=cooldown[slot];auto button=allowed&&c.showCooldown?Button64(ui,base,p,slot):uintptr_t(0);')
needle='  std::unordered_set<uintptr_t> frames;if(!Frames64';assert s.count(needle)==1;s=s.replace(needle,'  }\n  sampling.Configure(ui,selected,c,allowed);if(!sampling.Full(now))return observed|observedNative;\n  std::unordered_set<uintptr_t> frames;if(!Frames64')
s=s.replace('  for(auto bar:frames){uintptr_t unit=0;bool visible=false;if(!Hp64(bar,base,p,unit,visible))continue;','  unsigned count=0;for(auto bar:frames){uintptr_t unit=0;bool visible=false;if(!Hp64(bar,base,p,unit,visible))continue;++count;')
s=s.replace('  return observed|observedNative;\n }\n unsigned WriteBar','  sampling.Load(count);return observed|observedNative;\n }\n unsigned WriteBar')
needle='hpColors.Update(base,p.build,bar,unit,allowed,GetTickCount64(),c);hpDimensions.Update(base,p.build,bar,allowed?c.barWidthHp:0.f,allowed?c.barHeightHp:0.f,raw);int level=Level64(unit,base,p);bool hero=level>0;';assert s.count(needle)==1
sample='''
   const bool hero=Object64(ComponentLookup131::Slot(unit,0x558),base,p.heroVtable);
   const bool eligibleNow=OverheadEligible131(allowed,c.hideInvulnerableBars,OverheadUnitFlags131(unit));
   auto cached=world.find(bar);
   if(cached!=world.end()&&cached->second.host&&!OwnedChild64(cached->second.host,bar,base,p.simpleVtable))sampling.Invalidate(unit);
   uintptr_t bound=0;uint64_t identity=0;
   if(StatPaintPresentationKey131(bar,base+p.unitVtable,bound,identity)&&bound==unit&&!sampling.Unit(unit,identity,bar,hero,GetTickCount64(),c,allowed)){
    if(cached!=world.end()&&cached->second.host){if(!eligibleNow)HideWorldHost131(cached->second.host);else if(!cached->second.valueText[0].empty()||!cached->second.valueText[1].empty()||!cached->second.levelText.empty())MaintainWorldHost131(cached->second.host,bar);}
    return observed;
   }
   hpColors.Update(base,p.build,bar,unit,allowed,GetTickCount64(),c);hpDimensions.Update(base,p.build,bar,allowed?c.barWidthHp:0.f,allowed?c.barHeightHp:0.f,raw);const int level=hero?Level64(unit,base,p):0;
'''
s=s.replace(needle,sample);p.write_text(s,encoding='utf-8')
print('Integrated 300 ms sweeps, 200/400/800 ms sampling, selected/hero priority, 100 ms panel queries on both architectures')
