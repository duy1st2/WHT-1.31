from port3_research import *
for pid in [8740,32116]:
 b,base,bits=image(pid);print('NAME',pid)
 for m in re.finditer(b'\x8d[\x41\x59\x71\x79]\x34',b):
  a=m.start()
  if 0xb00000<a<0xc00000:print(hex(a));dis(b,base,bits,a-7,55)
for pid,a in [(24984,0xd09a90),(27336,0xd48550)]:
 b,base,bits=image(pid);print('WRAPPERS',pid)
 for m in re.finditer(b'\xe8',b[0x900000:0xb00000]):
  q=m.start()+0x900000
  if q+5+struct.unpack_from('<i',b,q+1)[0]==a: print(hex(q));dis(b,base,bits,q-22,45)
