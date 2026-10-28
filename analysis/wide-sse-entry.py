from port3_research import *
b,base,bits=image(8740);groups={}
for m in re.finditer(b'\xe8',b[:0x1400000]):
 a=m.start();t=a+5+struct.unpack_from('<i',b,a+1)[0]
 if 0x14af000<=t<0x14af660:groups.setdefault(t,[]).append(a)
for t,v in groups.items():print(hex(t),len(v),[hex(x) for x in v[:18]])
dis(b,base,bits,0x14af500,100)
