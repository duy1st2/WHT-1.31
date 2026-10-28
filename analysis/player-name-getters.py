from port3_research import *
for pid,a in [(24984,0x316560),(27336,0x383fe0)]:
 b,base,bits=image(pid);print(pid);dis(b,base,bits,a,35)
