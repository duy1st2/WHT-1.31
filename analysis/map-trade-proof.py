from port3_research import *
for pid in [32116,27336]:
 b,base,bits=image(pid);patterns=[b'\x8b\x89\xdc\x00\x00\x00\x8b\x45\x08'] if bits==32 else [b'\x48\x8b\x80\xd0\x00\x00\x00\x85\x48\x60']
 for pat in patterns:
  for m in re.finditer(re.escape(pat),b):print('GETTER TAIL',pid,hex(m.start()));dis(b,base,bits,m.start(),32)
for pid,a,n in [(8740,0x91b408,60),(32116,0x8e796b,52),(24984,0xa322c0,50),(27336,0xa7e117,48)]:
 b,base,bits=image(pid);print('SETTER TAIL',pid);dis(b,base,bits,a,n)
