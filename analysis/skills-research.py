from port3_research import *
b,base=pe(root.parent/'Game.dll')
for a,n in [(0x34fdc0,128),(0x34fe00,128),(0x28ecf0,180),(0x277fe0,180),(0x278090,180)]:
 print('OLD',hex(a));dis(b,base,32,a,n)
for pid in [8740,32116,24984,27336]:
 b,base,bits=image(pid);pat=struct.pack('<I',0xd0142);hits=[m.start() for m in re.finditer(re.escape(pat),b[:0x1800000])];print('NEW',pid,hex(base),bits,[hex(a) for a in hits])
