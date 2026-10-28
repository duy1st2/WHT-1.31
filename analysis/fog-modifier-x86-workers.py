from port3_research import *
b,base,bits=image(8740)
for a,n in [(0x8d0c10,340),(0x8d67d0,70),(0x8d31a0,50),(0x8f1470,50)]: print(hex(a));dis(b,base,bits,a,n)
