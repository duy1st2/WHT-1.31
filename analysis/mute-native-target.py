from port3_research import *
for pid,a in [(8740,0xb0a810),(32116,0xac9f10),(24984,0xc57020),(27336,0xc9a530)]:
 b,base,bits=image(pid);print('TARGET',pid,hex(a));dis(b,base,bits,a,90)
 if bits==32:
  ends=[m.start()+a for m in re.finditer(b'\xc2\x10\x00',b[a:a+0x4000])];print('RET16',list(map(hex,ends[:8])))
  for end in ends[:1]:dis(b,base,bits,end-55,65)
 else:
  pe=struct.unpack_from('<I',b,60)[0];rva,size=struct.unpack_from('<II',b,pe+24+112+24);pdata=[struct.unpack_from('<III',b,q) for q in range(rva,rva+size,12)]
  def rootentry(t):
   for _ in range(30):
    u=t[2]
    if not b[u]&32:return t
    t=struct.unpack_from('<III',b,u+4+((b[u+2]+1)&~1)*2)
   raise RuntimeError('chain')
  end=max(t[1] for t in pdata if rootentry(t)[0]==a);print('END',hex(end));dis(b,base,bits,end-110,110)
