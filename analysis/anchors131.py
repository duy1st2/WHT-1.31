import pathlib,pefile,capstone,re,collections,json
root=pathlib.Path(__file__).resolve().parents[1]
p=pefile.PE(str(root.parent/'WHT-1.29/runtime-test/Warcraft3-129/war3.exe'),fast_load=True); old=p.get_memory_mapped_image();base=p.OPTIONAL_HEADER.ImageBase
d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);d.detail=True
targets={'version_text':0x47e380,'chat_display':0x1f3020,'camera_set':0x1e30e0,'font_text':0x487a40,'font_set':0x487470,'font_align':0x487560,'font_width':0x486fa0,'texture_set':0x487c20,'texture_ctor':0x4864c0,'backdrop_ctor':0x48d510,'backdrop_set':0x488e10,'string_assign':0x68d70,'rel_point':0x473a10,'chat_open':0x22e320,'chat_submit':0x1ee2f0,'chat_text':0x481c70,'ui_get':0x1f0080,'clear':0x48b7b0,'fps_mode':0x24e810,'fog_set':0x56cd30,'main_action':0x16e1b0}
result={}
for pid in [14740,23884]:
 new=(root/f'analysis/memory-{pid}.bin').read_bytes(); entries={}
 for name,rva in targets.items():
  ins=[]
  for i in d.disasm(old[rva:rva+600],base+rva):
   code=[re.escape(bytes([x])) for x in i.bytes]
   if i.mnemonic.startswith(('call','j','loop')) and i.imm_size:
    for x in range(i.imm_offset,i.imm_offset+i.imm_size):code[x]=b'.'
   if i.disp_size and i.disp>=base:
    for x in range(i.disp_offset,i.disp_offset+i.disp_size):code[x]=b'.'
   if i.imm_size and i.operands and i.operands[-1].type==capstone.x86.X86_OP_IMM and base<=i.operands[-1].imm<base+len(old):
    for x in range(i.imm_offset,i.imm_offset+i.imm_size):code[x]=b'.'
   ins.append(b''.join(code))
   if i.mnemonic=='ret' and len(ins)>30:break
  hits=collections.Counter(); examples={}
  for start in range(0,len(ins)-6,3):
   pattern=b''.join(ins[start:start+7])
   if len(pattern)<18:continue
   matched=[m.start() for m in re.finditer(pattern,new,re.DOTALL)]
   if len(matched)>8:continue
   for pos in matched:
    pro=new.rfind(b'\x55\x8b\xec',max(0,pos-2500),pos+1)
    if pro>=0 and pro%16==0:hits[pro]+=1;examples.setdefault(pro,[]).append(hex(pos))
  entries[name]=[{'rva':hex(x),'votes':v,'anchors':examples[x]} for x,v in hits.most_common(5)]
  print(pid,name,entries[name],flush=True)
 result[str(pid)]=entries
(root/'analysis/anchors131.json').write_text(json.dumps(result,indent=2))
