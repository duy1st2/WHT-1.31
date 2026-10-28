from port3_research import *
for pid,a in [(32116,0x984360),(24984,0xae56f0),(27336,0xb2e7d0)]:
 b,base,bits=image(pid);print(pid);dis(b,base,bits,a,220)
