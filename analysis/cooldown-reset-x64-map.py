from port3_research import *
for pid,event in [(24984,0x40f3c3),(27336,0)]:
 b,base,bits=image(pid);pe=struct.unpack_from('<I',b,60)[0];rva,size=struct.unpack_from('<II',b,pe+24+112+24);pdata=[struct.unpack_from('<III',b,a) for a in range(rva,rva+size,12)]
 if not event:
  candidates=[]
  for m in re.finditer(bytes.fromhex('be010d00'),b[:0x1800000]):
   a=m.start()
   if bytes.fromhex('488d8b58010000') in b[a-40:a+40]:candidates.append(a)
  print('EVENTS',list(map(hex,candidates)));event=candidates[-1]
 def rootentry(t):
  for _ in range(20):
   u=t[2]
   if not b[u]&32:return t
   at=u+4+((b[u+2]+1)&~1)*2;t=struct.unpack_from('<III',b,at)
  raise RuntimeError('chain')
 start,end,u=rootentry(next(t for t in pdata if t[0]<=event<t[1]));end=max(t[1] for t in pdata if rootentry(t)[0]==start);print('ABILITY',pid,hex(start),hex(end));dis(b,base,bits,start,end-start)
 for m in re.finditer(b'[\xe8\xe9]',b):
  a=m.start()
  if a+5<len(b) and a+5+struct.unpack_from('<i',b,a+1)[0]==start:
   print('CALLER',hex(a));dis(b,base,bits,a-15,45)
   if 0xc00000<a<0xe00000:
    s,e,u=rootentry(next(t for t in pdata if t[0]<=a<t[1]));e=max(t[1] for t in pdata if rootentry(t)[0]==s);print('UNIT RESET',hex(s),hex(e));dis(b,base,bits,s,e-s)
    for mm in re.finditer(b'[\xe8\xe9]',b):
     q=mm.start()
     if q+5<len(b) and q+5+struct.unpack_from('<i',b,q+1)[0]==s:print('UNIT CALLER',hex(q));dis(b,base,bits,q-24,50)
