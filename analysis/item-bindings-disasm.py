from port3_research import *
for pid,create,getter in [(8740,0x8d13f0,0x8dd8b0),(32116,0x89ff30,0x8ac300),(24984,0x9e3fb0,0x9f2320),(27336,0xa2ff60,0xa3d9a0)]:
 b,base,bits=image(pid);print('PROFILE',pid);dis(b,base,bits,create,170);print('GETTYPE');dis(b,base,bits,getter,65)
