from port3_research import *
for pid,a in [(8740,0xafe670),(32116,0xabd570),(24984,0xc48c70),(27336,0xc8e9d0)]:
 b,base,bits=image(pid);print('CALL SETUP',pid);dis(b,base,bits,a,110)
 if bits==32:
  at=0xb0a810 if pid==8740 else 0xaca110;print('TARGET',hex(at));dis(b,base,bits,at,150)
