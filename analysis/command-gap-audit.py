from pathlib import Path
import re,json
root=Path(__file__).resolve().parents[1]
def commands(p):return set(re.findall(r'"(#[a-zA-Z0-9_]+)"',p.read_text(encoding='utf-8-sig')))
a=commands(root.parent/'WHT/native/gameplay_commands.hpp');b=commands(root.parent/'WHT-1.29/native/gameplay_commands.hpp')
active=commands(root/'native/version131.cpp')|commands(root/'native/version64.cpp')
for p in (root/'native').glob('*131.hpp'):active|=commands(p)
print(json.dumps(sorted((a&b)-active),indent=2))
