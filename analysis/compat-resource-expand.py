from pathlib import Path
import shutil
r=Path('WHT-1.31');backup=sorted((r/'analysis').glob('before-wht-display-aliases-*'))[-1]
for file,suf in [('version131.cpp','131'),('version64.cpp','64')]:
 p=r/'native'/file;shutil.copy2(p,backup/file);s=p.read_text(encoding='utf-8-sig');lines=s.splitlines();matches=[line for line in lines if 'ViewCommandFor131' in line];assert len(matches)==1;old=matches[0];new=old.replace('return wht::ApplyViewCommand131','const bool ok=wht::ApplyViewCommand131',1);end=f'[&](){{Reload{suf}();}});}}';assert new.endswith(end);new=new[:-1]+f'if(ok&&option->field==&wht::Config::showResource&&config{suf}.showResource){{resources{suf}.expanded=true;resources{suf}.next=0;}}return ok;}}';s=s.replace(old,new,1);p.write_text(s,encoding='utf-8-sig')
p=r/'README.md';s=p.read_text(encoding='utf-8-sig');s+='\nResource display follow-up: enabling #trade/#res/#resource (including #showresource alias) also expands the existing resource board and refreshes it immediately, matching WHT Show Resource command behavior after a user previously collapsed it.\n';p.write_text(s,encoding='utf-8-sig')
