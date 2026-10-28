from port3_research import *
b,base,bits=image(8740)
for m in re.finditer(re.escape(b'\xf3\x0f\x51'),b[:0x1700000]):
 a=m.start();start=b.rfind(b'\x55\x8b\xec',max(0,a-600),a)
 if start<0:continue
 # list functions using scalar sqrt and at least six div/mul instructions
 d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);ins=list(d.disasm(b[start:a+220],base+start))
 if sum(i.mnemonic in ['divss','mulss'] for i in ins)>=5:
  print('CANDIDATE',hex(start),'sqrt',hex(a));dis(b,base,bits,start,a+200-start)
