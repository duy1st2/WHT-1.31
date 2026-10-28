from port3_research import *
for pid in [8740,32116,24984,27336]:
 b,base,bits=image(pid);needle=b'.?AVCMapSetupWar3@@\0';at=b.find(needle);td=at-(8 if bits==32 else 16);print(pid,'type',hex(td),'refs:')
 # COL embeds type descriptor pointer on x86 / RVA on x64.
 p=struct.pack('<I',base+td if bits==32 else td)
 for m in re.finditer(re.escape(p),b):
  q=m.start();col=q-12
  if bits==64 and struct.unpack_from('<I',b,col)[0]!=1:continue
  if bits==32 and struct.unpack_from('<I',b,col)[0]!=0:continue
  refs=[x.start()+bits//8 for x in re.finditer(re.escape(struct.pack('<I' if bits==32 else '<Q',base+col)),b)]
  print('col',hex(col),'offset',struct.unpack_from('<I',b,col+4)[0],'tables',[hex(x) for x in refs])
