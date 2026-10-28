from port3_research import *
b,base,bits=image(8740)
for a,n in [(0xaa0cd0,60),(0xf45190,70),(0x9c33d0,180)]:print('DIS',hex(a));dis(b,base,bits,a,n)
for m in re.finditer(b'\xe8',b[:0x1900000]):
 a=m.start()
 if a+5+struct.unpack_from('<i',b,a+1)[0]==0xaa0cd0:print('ENCODE CALL',hex(a))
