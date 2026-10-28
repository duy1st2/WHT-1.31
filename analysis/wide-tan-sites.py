from port3_research import *
b,base,bits=image(8740)
for m in re.finditer(b'\xe8',b[:0x1900000]):
 a=m.start()
 if a+5+struct.unpack_from('<i',b,a+1)[0] in [0xf47560,0xf47390]:
  if a+5+struct.unpack_from('<i',b,a+1)[0]==0xf47560:print('TAN CALL',hex(a));dis(b,base,bits,a-35,110)
