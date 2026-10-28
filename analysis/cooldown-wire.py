from pathlib import Path
import re,json,hashlib
root=Path(__file__).resolve().parents[1]
for rel,cmd,suffix,ui,window in [('native/version131.cpp','command','131','currentUi131','gameWindow131'),('native/version64.cpp','cmd','64','ui64','window64')]:
 p=root/rel;s=p.read_text(encoding='utf-8');s=s.replace('#include "unit_model131.hpp"','#include "unit_model131.hpp"\n#include "no_cooldown131.hpp"');s=s.replace(f'wht::RepeatMessage131 repeatMessage{suffix};',f'wht::RepeatMessage131 repeatMessage{suffix};\nwht::NoCooldown131 noCooldown{suffix};')
 selected=re.search(r'ReadUnitModel131\(base'+suffix+r',profile'+suffix+r'->build,\[&\]\(\)\{return (.*?);\}',s).group(1)
 apply=f'wht::ResetSelectedCooldown131(base{suffix},profile{suffix}->build,[&](){{return {selected};}})'
 handler=f' if({cmd}==L"#nocd"||{cmd}==L"#yescd"){{std::wstring args;std::getline(in,args);std::string report;const bool ok=({cmd}==L"#yescd"||(settings{suffix}&&Allowed{suffix}()))&&noCooldown{suffix}.Command({cmd},args,GetTickCount64(),[&](){{return {apply};}},report);if(report.empty())report="No Cooldown: settings or session disabled";wht::Log(folder{suffix},report);wht::ShowNotification131(base{suffix},profile{suffix}->build,{ui},report);return ok;}}\n'
 at=s.index(f' if({cmd}==L"#gmodel")');s=s[:at]+handler+s[at:]
 s=s.replace(f'repeatMessage{suffix}.Reset();',f'repeatMessage{suffix}.Reset();noCooldown{suffix}.Reset();')
 marker=f' repeatMessage{suffix}.Tick('
 at=s.index(marker);ready=f'!typed{suffix}&&!menu{suffix}.open&&!macro{suffix}.playing&&!macro{suffix}.recording&&!allSkills{suffix}.active&&!craft{suffix}.active&&!delivery{suffix}.active&&GetForegroundWindow()=={window}'
 tick=f' if(noCooldown{suffix}.Tick(Allowed{suffix}()&&settings{suffix},{ready},GetTickCount64(),[&](){{return {apply};}})){{const std::string report="No Cooldown: stopped; selection/session changed or native reset failed";wht::Log(folder{suffix},report);wht::ShowNotification131(base{suffix},profile{suffix}->build,{ui},report);}}\n'
 s=s[:at]+tick+s[at:];p.write_text(s,encoding='utf-8')
before={}
for client in (root/'runtime-test').iterdir():
 for arch in ['x86','x86_64']:
  folder=client/arch
  if folder.is_dir():
   for p in folder.iterdir():
    if p.is_file() and p.suffix.lower() in ['.ini','.log']:before[str(p)]=hashlib.sha256(p.read_bytes()).hexdigest()
(root/'analysis/no-cooldown-preserved-before.json').write_text(json.dumps(before,indent=2),encoding='utf-8')
