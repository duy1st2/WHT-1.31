from port3_research import *
for pid,a in [(8740,0xd60e30),(32116,0xd1bde0),(27336,0xf44400)]:
 b,base,bits=image(pid);print('ENUM',pid,hex(a));dis(b,base,bits,a,500)
