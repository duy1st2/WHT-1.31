from port3_research import *
for pid,near in [(8740,0xbb1b8b),(32116,0xb7112b)]:
 b,base,bits=image(pid);start=b.rfind(b'\x55\x8b\xec',near-512,near);print('RESET',pid,hex(start));dis(b,base,bits,start,350)
 for m in re.finditer(b'[\xe8\xe9]',b):
  a=m.start()
  if a+5<len(b) and a+5+struct.unpack_from('<i',b,a+1)[0]==start:print('CALLER',hex(a));dis(b,base,bits,a-20,40)
 print('ABILITYRESET');dis(b,base,bits,(0x3a25f0 if pid==8740 else 0x380b00),250)
