from port3_research import *
b,base,bits=image(8740)
for a,n in [(0xaa0620,180),(0xaa1b70,160)]:print('DIS',hex(a));dis(b,base,bits,a,n)
