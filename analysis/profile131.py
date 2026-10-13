import pathlib,capstone,re,json,struct
root=pathlib.Path(__file__).resolve().parents[1]
b=(root/'analysis/memory-23884.bin').read_bytes();new=(root/'analysis/memory-14740.bin').read_bytes();base=0x270000
d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);d.detail=True
targets={'ui_get':0xab9910,'camera_set':0xaac990,'alloc':0x275cd0,'frame_ctor':0xdbdc80,'font_ctor':0xdbbce0,'font_set':0xdbd060,'font_text':0xdbd630,'font_align':0xdbd140,'texture_ctor':0xdbbdb0,'texture_set':0xdbd7f0,'backdrop_ctor':0xdde070,'backdrop_set':0xdbea80,'string_assign':0x2aa6d0,'set_width':0xdb9f10,'set_height':0xdb9ca0,'abs_point':0xdb9d70,'rel_point':0xdb9e30,'text_frame_set':0xdc4410,'read_float':0xd23020,'archive_open':0x2c1d20,'file_info':0x2c16c0,'spell_remaining':0x380290,'timer_remaining':0xd256f0,'timer_remaining_alt':0xd25730,'button_ability':0xb2cfa0}
out={}
for name,rva in targets.items():
 ins=[]
 for i in d.disasm(b[rva:rva+160],base+rva):
  code=[re.escape(bytes([x])) for x in i.bytes]
  if i.imm_size and (i.mnemonic.startswith(('call','j')) or (i.operands[-1].type==capstone.x86.X86_OP_IMM and abs(i.operands[-1].imm)>0x10000)):
   for x in range(i.imm_offset,i.imm_offset+i.imm_size):code[x]=b'.'
  if i.disp_size and i.disp>=base:
   for x in range(i.disp_offset,i.disp_offset+i.disp_size):code[x]=b'.'
  ins.extend(code)
  if i.mnemonic=='ret':break
 hits=[]
 for n in [96,64,48,32,24,16,11]:
  pattern=b''.join(ins[:n]);hits=[m.start() for m in re.finditer(pattern,new,re.DOTALL) if m.start()%16==0]
  if len(hits)==1:break
 print(name,hex(rva),n,[hex(x) for x in hits][:30])
 out[name]={'12173':rva,'12164':hits[0] if len(hits)==1 else None,'signature73':b[rva:rva+8].hex(),'signature64':new[hits[0]:hits[0]+8].hex() if len(hits)==1 else None}
(root/'analysis/profile131.json').write_text(json.dumps(out,indent=2))
