from port3_research import *
for pid,table in [(8740,0x1806950),(32116,0x17bb980),(24984,0x1bb2f48),(27336,0x1be1f20)]:
 b,base,bits=image(pid);w=bits//8;at=table+(0x64 if bits==32 else 0xc8);fn=int.from_bytes(b[at:at+w],'little')-base;print('OWNER GETTER',pid,hex(fn));dis(b,base,bits,fn,22)
