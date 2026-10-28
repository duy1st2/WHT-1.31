from port3_research import *
b,base,bits=image(8740)
for name in [b'sqrt\0',b'acos\0',b'tan\0',b'tanf\0',b'atan\0']:
 print(name,[hex(m.start()) for m in re.finditer(re.escape(name),b)][:20])
# matrix builders often return this and ret16; inspect functions with all output matrix lanes
for m in re.finditer(re.escape(b'\xc2\x10\x00'),b[:0x1700000]):
 a=m.start();start=b.rfind(b'\x55\x8b\xec',max(0,a-700),a)
 if start<0:continue
 d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);d.detail=True;ins=list(d.disasm(b[start:a+3],base+start));stores=set()
 for i in ins:
  if i.mnemonic.startswith(('mov','fst')) and i.operands and i.operands[0].type==capstone.x86.X86_OP_MEM and i.operands[0].mem.base not in [capstone.x86.X86_REG_EBP,capstone.x86.X86_REG_ESP]:stores.add(i.operands[0].mem.disp)
 if 0x28 in stores and 0x38 in stores and 0 in stores and len(stores)>=6:print('MATRIX',hex(start),hex(a),'stores',sorted(stores))
