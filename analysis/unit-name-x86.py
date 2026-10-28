from port3_research import *
for pid in [8740,32116]:
 b,base,bits=image(pid);print('NAME',pid)
 for m in re.finditer(b'\x83\xc1[\x30-\x70]\x8b\xc1',b):
  a=m.start()
  if 0xb00000<a<0xc00000:print(hex(a));dis(b,base,bits,a-15,65)
 for m in re.finditer(b'\x8d[\x41\x59\x71\x79].\x85',b):
  a=m.start()
  if 0xba0000<a<0xbc0000 or 0xb60000<a<0xb80000:print(hex(a));dis(b,base,bits,a-5,50)
