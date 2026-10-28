from port3_research import *
for pid,create,getter in [(8740,0x8d13f0,0x8dd8b0),(32116,0x8a0160,0x8ac300),(24984,0x9e3fb0,0x9f2320),(27336,0xa2ff60,0xa3d9a0)]:
 b,base,bits=image(pid);d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 def targets(a,n):return [i.operands[0].imm-base for i in d.disasm(b[a:a+n],base+a) if i.mnemonic in ['call','jmp'] and i.operands[0].type==capstone.x86.X86_OP_IMM]
 worker,convert=targets(create,30 if bits==32 else 23)[:2];resolve=targets(getter,25)[0];print('PROFILE',pid,'WORKER',hex(worker),'CONVERT',hex(convert),'RESOLVE',hex(resolve));dis(b,base,bits,worker,280);print('HANDLE');dis(b,base,bits,convert,160);print('RESOLVE');dis(b,base,bits,resolve,110)
