from port3_research import *
b,base,bits=image(8740);a=b.rfind(b'\x55\x8b\xec',0xc5f400,0xc5f6c2);print(hex(a));dis(b,base,bits,a,270)
