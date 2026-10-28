from port3_research import *
for pid in [8740,24984]:
 b,base,bits=image(pid);p=struct.unpack_from('<I',b,60)[0];dd=p+24+(96 if bits==32 else 112);imp,size=struct.unpack_from('<II',b,dd+8);w=bits//8;print('PROFILE',pid)
 for a in range(imp,imp+size,20):
  oft,ts,fc,name,ft=struct.unpack_from('<5I',b,a)
  if not name:break
  j=0
  while oft:
   v=int.from_bytes(b[oft+j*w:oft+(j+1)*w],'little')
   if not v:break
   if v<len(b):
    n=b[v+2:b.find(b'\0',v+2)]
    if n in [b'sqrt',b'acos',b'tan',b'atan',b'sin',b'cos']:print(n,hex(ft+j*w))
   j+=1
