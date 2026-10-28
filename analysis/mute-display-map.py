from port3_research import *
for pid,call in [(8740,0xafe6c4),(32116,0xabd5c4),(24984,0xc48cd3),(27336,0xc8ea33)]:
 b,base,bits=image(pid);d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 if bits==32:
  start=b.rfind(b'\x55\x8b\xec',call-0x4000,call);print('DISPLAY',pid,hex(start));dis(b,base,bits,start,125)
 else:
  pe=struct.unpack_from('<I',b,60)[0];rva,size=struct.unpack_from('<II',b,pe+24+112+24);pdata=[struct.unpack_from('<III',b,a) for a in range(rva,rva+size,12)]
  def rootentry(t):
   for _ in range(30):
    u=t[2]
    if not b[u]&32:return t
    t=struct.unpack_from('<III',b,u+4+((b[u+2]+1)&~1)*2)
   raise RuntimeError('chain')
  start,end,u=rootentry(next(t for t in pdata if t[0]<=call<t[1]));end=max(t[1] for t in pdata if rootentry(t)[0]==start);print('DISPLAY',pid,hex(start),hex(end));dis(b,base,bits,start,125)
 # Show duration use and the native argument setup at the history/display boundary.
 print('HISTORY EDGE');dis(b,base,bits,call-(16 if bits==32 else 30),40)
 print('REFERENCES')
 for m in re.finditer(b'[\xe8\xe9]',b):
  a=m.start()
  if a+5<len(b) and a+5+struct.unpack_from('<i',b,a+1)[0]==start:print('CALLER',hex(a));dis(b,base,bits,a-24,40)
