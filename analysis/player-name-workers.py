from port3_research import *
for pid,a,n in [(27336,0xa7e960,65),(24984,0x316170,280),(32116,0x2aa6d0,180)]:
 b,base,bits=image(pid);print('PID',pid);dis(b,base,bits,a,n)
