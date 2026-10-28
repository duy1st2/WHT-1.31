from port3_research import *
b,base,bits=image(8740);d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);d.detail=True
for m in re.finditer(re.escape(b'\x55\x8b\xec'),b[0xaa0000:0xaa4000]):
 a=0xaa0000+m.start();ins=[]
 for i in d.disasm(b[a:a+200],base+a):
  ins.append(i)
  if i.mnemonic=='ret':break
 calls=[i.operands[0].imm-base for i in ins if i.mnemonic=='call' and i.operands[0].type==capstone.x86.X86_OP_IMM]
 if len(calls)==3 and len(set(calls))==1:print(hex(a),[hex(x) for x in calls]);dis(b,base,bits,a,sum(i.size for i in ins))

