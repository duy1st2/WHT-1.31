from port3_research import *
b,base,bits=image(8740)
for a,n in [(0x8ddd50,260),(0x8e4d30,320)]:print('NATIVE',hex(a));dis(b,base,bits,a,n)
