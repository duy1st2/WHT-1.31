from port3_research import *
b,base,bits=image(8740)
for a in [0x36dff0,0xd57460,0x114a0b0]:print('MATRIX',hex(a));dis(b,base,bits,a,420)
