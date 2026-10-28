from port3_research import *
old,oldbase=pe(root.parent/'WHT-1.29/runtime-test/Warcraft3-129/war3.exe');print('OLD DISPLAY');dis(old,oldbase,32,0x1f3020,160)
for pid,target in [(8740,0xb6a540),(32116,0xb2a260),(24984,0xcc0c20),(27336,0xd00b40)]:
 b,base,bits=image(pid);print('PID',pid,'HISTORY',hex(target))
 for m in re.finditer(b'[\xe8\xe9]',b):
  a=m.start()
  if a+5<len(b) and a+5+struct.unpack_from('<i',b,a+1)[0]==target:
   print('CALLER',hex(a));dis(b,base,bits,a-45,95)
