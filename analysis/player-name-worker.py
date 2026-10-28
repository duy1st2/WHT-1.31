from port3_research import *
b,base,bits=image(8740)
for a,n in [(0x2c08a0,190),(0x6c8490,110),(0x2c0b90,45)]:print('BODY',hex(a));dis(b,base,bits,a,n)
