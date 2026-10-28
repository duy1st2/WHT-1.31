from pathlib import Path
import re
r=Path('WHT-1.31');cmd=lambda p:set(re.findall(r'"(#[a-zA-Z0-9_]+)"',p.read_text(encoding='utf-8-sig')))
a=cmd(Path('WHT/native/runtime.cpp'))|cmd(Path('WHT/native/gameplay_commands.hpp'));active=cmd(r/'native/version131.cpp')|cmd(r/'native/version64.cpp')
for p in (r/'native').glob('*131.hpp'):active|=cmd(p)
print(sorted(a-active))
