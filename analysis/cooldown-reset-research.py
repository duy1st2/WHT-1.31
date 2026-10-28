from port3_research import *
old,oldbase=pe(root.parent/'WHT-1.29/runtime-test/Warcraft3-129/war3.exe');print('OLD RESET');dis(old,oldbase,32,0x2d8a10,420)
for pid in [8740,32116,24984,27336]:
 b,base,bits=image(pid);s=b.find(b'UnitResetCooldown\0');print('PID',pid,'NAME',hex(s))
 if bits==32:
  for m in re.finditer(b'\x8b\xce\xe8....\x8b\x45.\x85\xc0',b,re.S):
   a=m.start()
   if 0xa00000<a<0xd00000:print('RESET CANDIDATE',hex(a));dis(b,base,bits,a-25,90)
 if bits==32:
  for m in re.finditer(re.escape(struct.pack('<I',base+s)),b):dis(b,base,bits,m.start()-12,48)
 else:
  for m in re.finditer(b'\x48\x8d[\x05\x0d\x15\x1d\x25\x2d\x35\x3d]',b):
   a=m.start()
   if a+7+struct.unpack_from('<i',b,a+3)[0]==s:dis(b,base,bits,a-10,56)
