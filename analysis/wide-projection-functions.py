from port3_research import *
b,base,bits=image(8740)
for a in [0xffef30,0xfff240,0x1010ec0]:print('FUNCTION',hex(a));dis(b,base,bits,a,830)
