from port3_research import *
for pid,a in [(24984,0xd20630)]:
 b,base,bits=image(pid);print('WORKER',pid,hex(a));dis(b,base,bits,a,380)
