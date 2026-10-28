from port3_research import *
for pid,at in [(8740,0xb11c31),(32116,0xad1541),(24984,0xc5dd6a),(27336,0xca0f9a)]:
 b,base,bits=image(pid)
 if bits==32:start=b.rfind(b'\x55\x8b\xec',at-0x4000,at);end=at+50
 else:
  pe=struct.unpack_from('<I',b,60)[0];rva,size=struct.unpack_from('<II',b,pe+24+112+24);pdata=[struct.unpack_from('<III',b,a) for a in range(rva,rva+size,12)]
  def rootentry(t):
   for _ in range(30):
    u=t[2]
    if not b[u]&32:return t
    t=struct.unpack_from('<III',b,u+4+((b[u+2]+1)&~1)*2)
   raise RuntimeError('chain')
  start,end,u=rootentry(next(t for t in pdata if t[0]<=at<t[1]));end=max(t[1] for t in pdata if rootentry(t)[0]==start)
 print('PARENT',pid,hex(start),hex(end));dis(b,base,bits,start,min(260,end-start));print('CALLSITE');dis(b,base,bits,at-60,85)
