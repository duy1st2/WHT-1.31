from port3_research import *
for pid,a,codec in [(8740,0x9c9f00,0xaa0cd0),(32116,0x98aa90,0xa5fc90),(24984,0xaec0c0,0xbd8570),(27336,0xb34bc0,0xc1f040)]:
 b,base,bits=image(pid);d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 print('PROFILE',pid);dis(b,base,bits,codec,100)
 for i in d.disasm(b[a:a+130],base+a):
  for op in i.operands:
   if op.type==capstone.x86.X86_OP_MEM and op.mem.disp and (not op.mem.base or op.mem.base==capstone.x86.X86_REG_RIP):
    rv=op.mem.disp-base if not op.mem.base else i.address+i.size+op.mem.disp-base
    if 0<rv<len(b)-4:print('CONST',hex(rv),b[rv:rv+4].hex())
