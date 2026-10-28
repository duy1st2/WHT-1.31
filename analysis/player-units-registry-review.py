from port3_research import *
# Save actual complete disassembly for ABI/bounds review, without executing it.
b,base,bits=image(24984)
for a,n in [(0xf08610,0xe0),(0xf086f0,0x200)]: print('NATIVE',hex(a));dis(b,base,bits,a,n)
for pid,tag in [(8740,0xba40e0),(32116,0xb63730),(27336,0xd3fe10)]:
 b,base,bits=image(pid);print('UNIT CLASS',pid);dis(b,base,bits,tag,60)
