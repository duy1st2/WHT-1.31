from pathlib import Path
import json,hashlib
root=Path(__file__).resolve().parents[1]
p=root/'native/repeat_chat131.hpp';s=p.read_text(encoding='utf-8');s=s.replace('#include "resource131.hpp"','#include "resource131.hpp"\n#include "mute_chat_state131.hpp"');s=s.replace(' originalChatHistory131(object,sender,text,channel);',' if(!mutedChat131.Hidden(sender,channel))originalChatHistory131(object,sender,text,channel);')
old=' unsigned local=24;const auto* p=ResourceProfileFor131(build);if(allowed&&p&&ResourceCode131(base,*p)&&ResourceWorld131(base,*p,local))repeatChat131.Publish(true,local);else repeatChat131.Reset();'
new=' unsigned local=24;uintptr_t world=0;const auto* p=ResourceProfileFor131(build);if(allowed&&p&&ResourceCode131(base,*p)&&(world=ResourceWorld131(base,*p,local))){repeatChat131.Publish(true,local);mutedChat131.Publish(true,world,local);}else{repeatChat131.Reset();mutedChat131.Reset();}'
assert old in s;s=s.replace(old,new);p.write_text(s,encoding='utf-8')
for rel,cmd,suffix,ui in [('native/version131.cpp','command','131','currentUi131'),('native/version64.cpp','cmd','64','ui64')]:
 p=root/rel;s=p.read_text(encoding='utf-8');s=s.replace('#include "no_cooldown131.hpp"','#include "no_cooldown131.hpp"\n#include "mute_chat131.hpp"');s=s.replace('wht::repeatChat131.Reset();','wht::repeatChat131.Reset();wht::mutedChat131.Reset();')
 at=s.index(f' if({cmd}==L"#nhailai")');handler=f' if({cmd}==L"#mute"||{cmd}==L"#unmute"){{std::wstring args;std::getline(in,args);std::string report;const bool ok=wht::MuteChatCommand131(base{suffix},profile{suffix}->build,Allowed{suffix}()&&settings{suffix},{cmd},args,report);wht::Log(folder{suffix},report);wht::ShowNotification131(base{suffix},profile{suffix}->build,{ui},report);return ok;}}\n';s=s[:at]+handler+s[at:];p.write_text(s,encoding='utf-8')
before={}
for client in (root/'runtime-test').iterdir():
 for arch in ['x86','x86_64']:
  folder=client/arch
  if folder.is_dir():
   for p in folder.iterdir():
    if p.is_file() and p.suffix.lower() in ['.ini','.log']:before[str(p)]=hashlib.sha256(p.read_bytes()).hexdigest()
(root/'analysis/mute-chat-preserved-before.json').write_text(json.dumps(before,indent=2),encoding='utf-8')
