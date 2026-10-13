import pefile,capstone,pathlib,re,json,struct
root=pathlib.Path(__file__).resolve().parents[1]
old=pefile.PE(str(root.parent/'WHT-1.29/runtime-test/Warcraft3-129/war3.exe')); b=old.get_memory_mapped_image(); ob=old.OPTIONAL_HEADER.ImageBase
d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);d.detail=True
targets={'version_text':0x47e380,'chat_display':0x1f3020,'camera_set':0x1e30e0,'fog_set':0x56cd30,'frame_ctor':0x488120,'font_ctor':0x486400,'font_text':0x487a40,'set_width':0x473b40,'set_height':0x473830,'abs_point':0x473950,'rel_point':0x473a10,'alloc':0x40610,'ui_global':0xd3d7f8,'font_width':0x486fa0,'show':0x489110,'hide':0x488960,'chat_open':0x22e320,'chat_submit':0x1ee2f0,'fps_mode':0x24e810,'clear':0x48b7b0,'game_ui':0x1e8700}
result={}
for pid in [14740,23884]:
 new=(root/f'analysis/memory-{pid}.bin').read_bytes(); profile={}
 for name,rva in targets.items():
  if name=='ui_global':continue
  chunks=[]; span=0
  for i in d.disasm(b[rva:rva+180],ob+rva):
   pos=i.address-ob-rva; code=bytearray(i.bytes); mask=[True]*i.size
   if i.mnemonic.startswith(('call','j','loop')):
    if i.imm_size:
     for x in range(i.imm_offset,i.imm_offset+i.imm_size):mask[x]=False
   if i.disp_size and i.disp>=ob:
    for x in range(i.disp_offset,i.disp_offset+i.disp_size):mask[x]=False
   if i.imm_size and ob<=i.operands[-1].imm<ob+len(b) and i.operands[-1].type==capstone.x86.X86_OP_IMM:
    for x in range(i.imm_offset,i.imm_offset+i.imm_size):mask[x]=False
   chunks.extend(bytes([byte]) if valid else None for byte,valid in zip(code,mask)); span=pos+i.size
   if span>=96 or i.mnemonic=='ret':break
  candidates=[];used=0
  for length in [len(chunks),64,48,32,20,11]:
   pat=b''.join(re.escape(c) if c else b'.' for c in chunks[:length])
   hits=[m.start() for m in re.finditer(pat,new,re.DOTALL) if m.start()%16==0]
   if hits:candidates=hits;used=min(length,len(chunks));break
  profile[name]={'old':hex(rva),'length':used,'matches':[hex(x) for x in candidates[:50]]}
  print(pid,name,used,[hex(x) for x in candidates[:18]])
 result[str(pid)]=profile
(root/'analysis/matched129.json').write_text(json.dumps(result,indent=2))
