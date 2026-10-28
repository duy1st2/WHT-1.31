from port3_research import *
b,base,bits=image(8740);a=b.rfind(b'\x55\x8b\xec',0xd53000,0xd533dc);print('PROJECTION',hex(a));dis(b,base,bits,a,320)
for m in re.finditer(b'\xe8',b[:0x1900000]):
 at=m.start()
 if at+5+struct.unpack_from('<i',b,at+1)[0]==a:print('CALLER',hex(at));dis(b,base,bits,at-35,65)
