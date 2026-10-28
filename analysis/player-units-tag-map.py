from port3_research import *
for pid in [8740,32116,24984,27336]:
 b,base,bits=image(pid);print('TAG',pid)
 for m in re.finditer(b'\xb8\x75\x33\x77\x2b\xc3',b):
  a=m.start();print(hex(a))
  for q in re.finditer(b'\xe8',b):
   s=q.start()
   if s+5+struct.unpack_from('<i',b,s+1)[0]==a:print('call',hex(s));dis(b,base,bits,s-22,55)
