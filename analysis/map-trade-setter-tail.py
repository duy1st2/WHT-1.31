from port3_research import *
for pid,a in [(8740,0x91b330),(32116,0x8e78a0),(24984,0xa321d0),(27336,0xa7e060)]:
 b,base,bits=image(pid);print('SETTER',pid);dis(b,base,bits,a,340)
