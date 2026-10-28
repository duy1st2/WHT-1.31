from port3_research import *
b,base,bits=image(8740)
for a in [0xf47390,0xf45a10,0x10357f0]:print('FUNC',hex(a));dis(b,base,bits,a,105)
