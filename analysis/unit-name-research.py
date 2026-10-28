from port3_research import *
for pid in [8740,32116,24984,27336]:
 b,base,bits=image(pid);print('NAME',pid)
 pats=[b'\x8d\x59\x30',b'\x8d\x71\x30',b'\x48\x8d\x59\x5c'] if bits==32 else [b'\x48\x8d\x59\x5c']
 for pat in pats:
  for m in re.finditer(re.escape(pat),b):
   a=m.start()
   if 0x900000<a<0xe00000:
    print(hex(a));dis(b,base,bits,a-6,60)
