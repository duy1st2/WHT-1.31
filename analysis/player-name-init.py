from port3_research import *
b,base,bits=image(8740);target=0x2c08a0
for m in re.finditer(b'\xe8',b[0x6b0000:0x6f0000]):
 a=0x6b0000+m.start()
 if a+5+struct.unpack_from('<i',b,a+1)[0]==target: print(hex(a));dis(b,base,bits,a-18,45)
