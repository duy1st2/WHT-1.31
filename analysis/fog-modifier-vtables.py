from port3_research import *
for pid in [8740,32116,24984,27336]:
 b,base,bits=image(pid);w=bits//8;td=b.find(b'.?AVCFogModifier@@\0')-2*w;needle=struct.pack('<I',td if bits==64 else base+td);print('VT',pid)
 for m in re.finditer(re.escape(needle),b):
  col=m.start()-12
  if struct.unpack_from('<I',b,col)[0]!=(1 if bits==64 else 0) or struct.unpack_from('<I',b,col+4)[0]!=0:continue
  for v in re.finditer(re.escape(struct.pack('<Q' if bits==64 else '<I',base+col)),b):
   at=v.start()+w;print(hex(at),[hex(int.from_bytes(b[at+i*w:at+(i+1)*w],'little')-base) for i in range(32)])
