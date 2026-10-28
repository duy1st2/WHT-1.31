from pathlib import Path
r=Path(__file__).resolve().parents[1]
for file,suf,cmd,ui in [('version131.cpp','131','command','currentUi131'),('version64.cpp','64','cmd','ui64')]:
 p=r/'native'/file;s=p.read_text(encoding='utf-8-sig');s=s.replace('#include "resource_transfer_network131.hpp"','#include "resource_transfer_network131.hpp"\n#include "alliance_spam131.hpp"',1)
 s=s.replace(f'wht::RepeatMessage131 repeatMessage{suf};',f'wht::RepeatMessage131 repeatMessage{suf};\nwht::AllianceSpam131 allianceSpam{suf};',1)
 marker=f' if({cmd}==L"#repeat")'
 command=f' if({cmd}==L"#spam"){{std::wstring args;std::getline(in,args);std::string report;const bool ok=allianceSpam{suf}.Command(args,settings{suf},[&](unsigned slot,wht::AllianceSpamContext131& out){{return wht::ReadAllianceSpam131(base{suf},profile{suf}->build,slot,out);}},report);wht::Log(folder{suf},report);wht::ShowNotification131(base{suf},profile{suf}->build,{ui},report);return ok;}}\n'
 assert marker in s;s=s.replace(marker,command+marker,1)
 s=s.replace(f'repeatMessage{suf}.Reset();',f'repeatMessage{suf}.Reset();allianceSpam{suf}.Reset();')
 marker=f' repeatMessage{suf}.Tick('
 at=s.index(marker);end=s.index('\n',at)
 ready=s[at:end].split(f'Allowed{suf}()&&settings{suf},',1)[1].rsplit(',GetTickCount64()',1)[0]
 tick=f'\n {{std::string report;if(!allianceSpam{suf}.Tick(Allowed{suf}()&&settings{suf},{ready},GetTickCount64(),[&](unsigned slot,wht::AllianceSpamContext131& out){{return wht::ReadAllianceSpam131(base{suf},profile{suf}->build,slot,out);}},[&](unsigned slot,uint32_t flags){{return wht::SendAllianceSpam131(base{suf},profile{suf}->build,slot,flags);}},report)&&!report.empty()){{wht::Log(folder{suf},report);wht::ShowNotification131(base{suf},profile{suf}->build,{ui},report);}}}}'
 s=s[:end]+tick+s[end:];p.write_text(s,encoding='utf-8-sig')
p=r/'AGENTS.md';s=p.read_text();s+='\nUser scope update (2026-10-28): if WHT-1.29 lacks an implementation, use\nthe actual WHT implementation as the reference. Record source evidence;\nplaceholder-only handlers still do not establish a working feature.\n';p.write_text(s)
