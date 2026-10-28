from port3_research import *
b,base,bits=image(32116);prefix=image(8740)[0][0xd53360:0xd53377];print('32 match',[hex(m.start()) for m in re.finditer(re.escape(prefix),b)])
for pid,a in [(24984,0xef7e20),(27336,0xf33e20)]:
 b,base,bits=image(pid);print('64',pid);dis(b,base,bits,a,360)
