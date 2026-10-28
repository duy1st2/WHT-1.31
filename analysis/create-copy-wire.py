from pathlib import Path
import re,json,hashlib
root=Path(__file__).resolve().parents[1]
for rel,cmd,suffix,ui in [('native/version131.cpp','command','131','currentUi131'),('native/version64.cpp','cmd','64','ui64')]:
 p=root/rel;s=p.read_text(encoding='utf-8');s=s.replace('#include "mute_chat131.hpp"','#include "mute_chat131.hpp"\n#include "create_unit131.hpp"')
 selected=re.search(r'ReadUnitModel131\(base'+suffix+r',profile'+suffix+r'->build,\[&\]\(\)\{return (.*?);\}',s).group(1)
 cursor=re.search(r'TeleportCommand131\(args,\[&\]\(wht::MouseWorldPosition131& out\)\{(.*?)\},\[&\]',s).group(1)
 at=s.index(f' if({cmd}==L"#tele")');line=f' if({cmd}==L"#create"||{cmd}==L"#copy"){{std::wstring args;std::getline(in,args);std::string report;const bool ok=settings{suffix}&&Allowed{suffix}()&&wht::CreateOrCopyUnitCommand131(base{suffix},profile{suffix}->build,{cmd},args,[&](){{return {selected};}},[&](wht::MouseWorldPosition131& out){{{cursor}}},report);if(report.empty())report="Create/Copy Unit: settings or session disabled";wht::Log(folder{suffix},report);wht::ShowNotification131(base{suffix},profile{suffix}->build,{ui},report);return ok;}}\n';s=s[:at]+line+s[at:];p.write_text(s,encoding='utf-8')
before={}
for client in (root/'runtime-test').iterdir():
 for arch in ['x86','x86_64']:
  folder=client/arch
  if folder.is_dir():
   for p in folder.iterdir():
    if p.is_file() and p.suffix.lower() in ['.ini','.log']:before[str(p)]=hashlib.sha256(p.read_bytes()).hexdigest()
(root/'analysis/create-copy-unit-preserved-before.json').write_text(json.dumps(before,indent=2),encoding='utf-8')
