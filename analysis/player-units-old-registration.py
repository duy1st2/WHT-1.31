from port3_research import *
b,base=pe(root.parent/'WHT-1.29/runtime-test/Warcraft3-129/war3.exe')
for name in [b'GroupEnumUnitsOfPlayer\0',b'CreateGroup\0']:
 p=b.find(name);needle=struct.pack('<I',base+p);print(name,hex(p))
 for m in re.finditer(re.escape(needle),b):
  a=m.start();print('ref',hex(a));dis(b,base,32,a-16,40)
