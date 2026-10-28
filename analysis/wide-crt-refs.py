from port3_research import *
b,base,bits=image(8740)
for at in [0x1919140,0x191a761,0x19244d1,0x19204b0]:
 print('STRING',hex(at),b[at:at+16]);refs=[m.start() for m in re.finditer(re.escape(struct.pack('<I',base+at)),b)];print([hex(x) for x in refs]);
 for a in refs[:3]:dis(b,base,bits,a-12,65)
