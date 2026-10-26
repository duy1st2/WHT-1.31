import sys,re,struct
sys.path.insert(0,'WHT-1.31/analysis');from port3_research import *
def tables(b,base,bits,name):
 out=[];needle=name.encode()+b'\0'
 for match in re.finditer(re.escape(needle),b):
  td=match.start()-(8 if bits==32 else 16)
  for m in re.finditer(re.escape(struct.pack('<I',td+(base if bits==32 else 0))),b):
   col=m.start()-12
   if col<0:continue
   sig,off,cd=struct.unpack_from('<III',b,col)
   if sig!=(0 if bits==32 else 1) or off or cd:continue
   for v in re.finditer(re.escape(struct.pack('<I' if bits==32 else '<Q',base+col)),b):out.append(v.start()+bits//8)
 return out
if __name__=='__main__':
 for pid in [14740,23884,6688,29928]:
  b,base,bits=image(pid)
  for name in ['.?AVCGlueMgr@@','.?AVCModel@@','.?AVCModelFrame@@']:
   ts=tables(b,base,bits,name);print(pid,name,[hex(t) for t in ts])
   if name=='.?AVCGlueMgr@@':
    for t in ts:
     print('GLUEVT',[(j,hex(struct.unpack_from('<I' if bits==32 else '<Q',b,t+j*(bits//8))[0]-base)) for j in [0,3,12]])
