from port3_research import *
b,base,bits=image(8740);p=struct.unpack_from('<I',b,60)[0];dd=p+24+96;exp,size=struct.unpack_from('<II',b,dd);print('exports',hex(exp),size)
if exp:
 nfunc,names,fa,na,oa=struct.unpack_from('<5I',b,exp+20)
 for j in range(names):
  a=struct.unpack_from('<I',b,na+j*4)[0];name=b[a:b.find(b'\0',a)]
  if any(s in name for s in [b'sqrt',b'tan',b'acos',b'Perspective']):
   ord=struct.unpack_from('<H',b,oa+j*2)[0];print(name,hex(struct.unpack_from('<I',b,fa+ord*4)[0]))
for at in [0x1875a34,0x1875a9c]:
 print('str',hex(at),'refs',[hex(m.start()) for m in re.finditer(re.escape(struct.pack('<I',base+at)),b)])
