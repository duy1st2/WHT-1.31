from port3_research import *
for pid,target in [(8740,0xc18630),(32116,0xbd6530)]:
 b,base,bits=image(pid);print(pid);dis(b,base,bits,target,65)
 for m in re.finditer(b'\xe8',b[:0x1700000]):
  a=m.start()
  if a+5+struct.unpack_from('<i',b,a+1)[0] in [target,target+0xb0]:
   print('CALL',hex(a));dis(b,base,bits,a-24,65)

