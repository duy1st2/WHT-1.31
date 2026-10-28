from port3_research import *
b,base=pe(root.parent/'Game.dll')
for rva in [0x34fdc0,0x34fe00,0x28ecf0,0x277fe0]:
 print('REF',hex(rva))
 for m in re.finditer(re.escape(struct.pack('<I',base+rva)),b):
  a=m.start();print(hex(a),b[max(0,a-24):a+24].hex())
