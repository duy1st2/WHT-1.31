from port3_research import *
b,base,bits=image(8740)
for a,n in [(0xb59800,1100),(0xbc9010,180),(0xc18630,200)]:
 print('BODY',hex(a));dis(b,base,bits,a,n)
