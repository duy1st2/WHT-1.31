from pathlib import Path
import shutil
r=Path('WHT-1.31');p=r/'native/command_feedback131.hpp';backup=sorted((r/'analysis').glob('before-create-item-*'))[-1];shutil.copy2(p,backup/p.name);s=p.read_text(encoding='utf-8-sig');marker=' for(const auto* handled:{';assert marker in s;s=s.replace(marker,marker+'L"#item",L"#copyitem",L"#create",L"#copy",',1);p.write_text(s,encoding='utf-8-sig')
p=r/'analysis/create-item-port.md';s=p.read_text();s=s.replace('No success is inferred from\na nonzero handle alone.','No success is inferred from\na nonzero handle alone. Creation handlers publish their own confirmed-count\nreport; generic command feedback is suppressed for item/unit creation and copy\nto prevent a second misleading OK line.');p.write_text(s)
