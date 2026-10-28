from port3_research import *
import hashlib
refs=[root.parent/'WHT/native/gameplay_commands.hpp',root.parent/'WHT-1.29/native/gameplay_commands.hpp'];(root/'analysis/fog-modifier-reference-evidence.json').write_text(json.dumps({'references':{str(p):hashlib.sha256(p.read_bytes()).hexdigest() for p in refs},'scope':'Both #mh create a local-player visible(4) fog modifier over GetWorldBounds with shared vision true, afterShared false; Start to enable, Stop and Destroy to disable.'},indent=2))
b,base=pe(root.parent/'WHT-1.29/runtime-test/Warcraft3-129/war3.exe')
for name in [b'GetWorldBounds\0',b'CreateFogModifierRect\0',b'FogModifierStart\0',b'FogModifierStop\0',b'DestroyFogModifier\0']:
 p=b.find(name);print(name,hex(p))
 for m in re.finditer(re.escape(struct.pack('<I',base+p)),b[:0x100000]):
  a=m.start();fn=struct.unpack_from('<I',b,a+5)[0]-base;print('native',hex(fn));dis(b,base,32,fn,180)
