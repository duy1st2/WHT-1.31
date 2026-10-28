from port3_research import *
b,base,bits=image(24984)
for a,n in [(0x9e3680,700),(0x9ea800,55),(0x9e63a0,90)]:print('NATIVE',hex(a));dis(b,base,bits,a,n)
