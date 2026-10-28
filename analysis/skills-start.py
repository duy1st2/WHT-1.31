from port3_research import *
for pid,call in [(8740,0xb59bc9),(32116,0xb19a27)]:
 b,base,bits=image(pid);a=b.rfind(bytes.fromhex('558bec'),call-1500,call);print(pid,'start',hex(a));dis(b,base,bits,a,70)
 print('hexblock',b[call-80:call+20].hex())
