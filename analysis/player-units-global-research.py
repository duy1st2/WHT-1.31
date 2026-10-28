from port3_research import *
for pid,start in [(8740,0xb20a10),(32116,0xae0220),(24984,0xc6ecd0),(27336,0xcb1c40)]:
 b,base,bits=image(pid);print('LIST',pid);dis(b,base,bits,start,105)
b,base,bits=image(24984);print('ENUM BODY');dis(b,base,bits,0xf086f0,440)
