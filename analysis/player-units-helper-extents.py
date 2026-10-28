from port3_research import *
for pid,a in [(32116,0xd1bde0),(27336,0xf444e0)]:
 b,base,bits=image(pid);print(pid);dis(b,base,bits,a,0x290)
