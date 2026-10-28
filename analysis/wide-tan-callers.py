from port3_research import *
b,base,bits=image(8740)
for a in [0x4f6a30,0x4fdb50,0x923d50]:print('FUNCTION',hex(a));dis(b,base,bits,a,650)
