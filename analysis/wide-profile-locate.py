from port3_research import *
b,base,bits=image(27336);prefix=image(24984)[0][0xef7e50:0xef7e87];hits=[m.start() for m in re.finditer(re.escape(prefix),b)];print('x64 match',[hex(x) for x in hits])
for a in hits:dis(b,base,bits,a,80)
# x86 reference verification and native validation tail
b,base,bits=image(8740);dis(b,base,bits,0xd5349c,22)
