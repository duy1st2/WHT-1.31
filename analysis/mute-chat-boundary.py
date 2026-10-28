from port3_research import *
old,base=pe(root.parent/'WHT-1.29/runtime-test/Warcraft3-129/war3.exe');print('OLD TAIL');dis(old,base,32,0x1f31c0,360)
for pid,a,n in [(8740,0xb6a540,0x207),(32116,0xb2a260,0x207),(24984,0xcc0c20,0x239),(27336,0xd00b40,0x239)]:
 b,base,bits=image(pid);print('DISPLAY/HISTORY',pid);dis(b,base,bits,a,n)
 print('VERIFIED TAIL');dis(b,base,bits,a+(0x1ad if pid==8740 else 0x199 if pid==32116 else 0x1c0 if pid==24984 else 0x1ab),180)
