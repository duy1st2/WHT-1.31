from port3_research import *
b,base,bits=image(8740);dis(b,base,bits,0x14d6180,160)
# direct callers entering this CRT range
for m in re.finditer(b'\xe8',b[:0x1700000]):
 a=m.start();t=a+5+struct.unpack_from('<i',b,a+1)[0]
 if 0x14d6180<=t<0x14d6266:print('CALL',hex(a),hex(t));dis(b,base,bits,a-18,65)
