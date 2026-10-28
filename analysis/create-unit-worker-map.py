from port3_research import *
for pid,marker in [(8740,0xbbec75),(32116,0xb7e031),(24984,0xd20630),(27336,0xd5f160)]:
 b,base,bits=image(pid);start=b.rfind(b'\x55\x8b\xec',marker-0x2000,marker) if bits==32 else marker
 print('WORKER',pid,hex(start));dis(b,base,bits,start,68)
 for m in re.finditer(b'[\xe8\xe9]',b):
  a=m.start()
  if a+5<len(b) and a+5+struct.unpack_from('<i',b,a+1)[0]==start:
   if 0x880000<a<0xb00000 and (bits==64 or b[a+5:a+8]==bytes.fromhex('6a0150')):
    print('WRAPPER CALL',hex(a));dis(b,base,bits,a-45,85)
    if bits==32:
     s=b.rfind(b'\x55\x8b\xec',a-512,a);print('WRAPPER START',hex(s));dis(b,base,bits,s,180)
