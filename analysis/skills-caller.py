from port3_research import *
b,base=pe(root.parent/'Game.dll');dis(b,base,32,0x373a50,310)
for pid in [8740,32116,24984,27336]:
 b,base,bits=image(pid);print(pid,len(b),b[0xbb09f0:0xbb09f8].hex());print('constant',len(list(re.finditer(re.escape(bytes.fromhex('42010d00')),b))))
