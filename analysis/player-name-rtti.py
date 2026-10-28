from port3_research import *
for pid in [8740,32116,24984,27336]:
 b,base,bits=image(pid);w=bits//8;print('PID',pid)
 for name in [b'CNetPlayer',b'CPlayer',b'CGameClient']:
  pat=b'.?AV'+name+b'@@\0';pos=b.find(pat)
  if pos<0:continue
  td=pos-2*w
  for m in re.finditer(re.escape(struct.pack('<I',base+td if bits==32 else td)),b):
   c=m.start()-12
   if c<0 or struct.unpack_from('<I',b,c)[0]!=(1 if bits==64 else 0):continue
   for v in re.finditer(re.escape(struct.pack('<I' if bits==32 else '<Q',base+c)),b):
    vt=v.start()+w
    if struct.unpack_from('<I' if bits==32 else '<Q',b,vt)[0]>base:
     print(name.decode(),hex(vt),[hex(struct.unpack_from('<I' if bits==32 else '<Q',b,vt+n*w)[0]-base) for n in range(12)])
