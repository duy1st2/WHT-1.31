from port3_research import *
for pid,get,wrapper in [(8740,0xbac220,0x8e3e60),(32116,0xb6b690,0x8b2570),(24984,0xd09a90,0x9f9760),(27336,0xd48550,0xa44dd0)]:
 b,base,bits=image(pid);d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 i=next(i for i in d.disasm(b[get:get+40],get+base) if i.mnemonic=='call');a=i.operands[0].imm-base
 print(pid,'getter',hex(get),'init',hex(a),'wrapper',hex(wrapper));dis(b,base,bits,wrapper,50);dis(b,base,bits,a,140)
