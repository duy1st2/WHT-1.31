from port3_research import *
b,base,bits=image(8740)
for a in [0xf01230,0x1015a20,0xd533b0,0xcf1350]:print('CANDIDATE',hex(a));dis(b,base,bits,a,220)
