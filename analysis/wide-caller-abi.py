from port3_research import *
for pid,a in [(24984,0x10f6399),(27336,0x1132df9)]:
 b,base,bits=image(pid);print(pid);dis(b,base,bits,a-40,95)
