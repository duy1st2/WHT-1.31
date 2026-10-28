from port3_research import *
for pid,enum,tag in [(8740,0xd60d80,0xba40e0),(32116,0xd1bd30,0xb63730)]:
 b,base,bits=image(pid);print('ENUM',pid);dis(b,base,bits,enum,230);print('TAG');dis(b,base,bits,tag,65)
