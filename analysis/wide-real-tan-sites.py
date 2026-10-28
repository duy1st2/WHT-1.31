from port3_research import *
b,base,bits=image(8740)
for a in [0xca6ee4,0xcd4b56,0xcd4f52,0xd533dc,0xfe4618,0x1010f6b]:print('TAN',hex(a));dis(b,base,bits,a-38,120)
