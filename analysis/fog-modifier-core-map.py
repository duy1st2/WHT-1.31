from port3_research import *
specs=[(8740,0x8d0c10,0x8d67d0),(32116,0x89f9a0,0x8a5210),(24984,0x9e3680,0x9ea800),(27336,0xa2f690,0xa36050)]
for pid,create,start in specs:
 b,base,bits=image(pid);d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 calls=lambda a,n:[i.operands[0].imm-base for i in d.disasm(b[a:a+n],base+a) if i.mnemonic=='call' and i.operands[0].type==capstone.x86.X86_OP_IMM]
 print(pid,[hex(v) for v in calls(create,130)[:2]],'fog',hex(calls(start,30)[0]))
 if pid==24984:
  target=calls(create,130)[1]
  for m in re.finditer(b'\xe8',b[0x900000:0xb00000]):
   a=0x900000+m.start()
   if a+5+struct.unpack_from('<i',b,a+1)[0]==target and b[a+5:a+10]==bytes.fromhex('4885c07411'):
    print('REMOVE?',hex(a-4));dis(b,base,bits,a-4,40)
