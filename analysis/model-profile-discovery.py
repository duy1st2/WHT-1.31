from port3_research import *
for pid in [24984,27336,8740,32116]:
 b,base,bits=image(pid);w=bits//8
 def ptr(a):return int.from_bytes(b[a:a+w],'little')
 def typename(vt):
  col=ptr(vt-w)-base
  td=int.from_bytes(b[col+12:col+16],'little')-(base if bits==32 else 0)
  return b[td+2*w:td+2*w+120].split(b'\0')[0]
 if pid==24984:
  names=[typename(0x1c720a0),typename(0x1c736d8)];print('NAMES',names)
 print('PID',pid,'BASE',hex(base))
 if bits==32:
  lo=0xef0000 if pid==8740 else 0xeb0000;hi=lo+0x30000
  for m in re.finditer(b'\x89[\x40-\x47]\x58',b[lo:hi]):
   print('FILENAME CANDIDATE');dis(b,base,bits,lo+m.start()-10,50)
  for m in re.finditer(b'\x8b\x4e(.)\xe8....\xc7\x46\\1\x00\x00\x00\x00',b[lo:hi],re.S):
   print('RESOURCE RELEASE',hex(lo+m.start()));dis(b,base,bits,lo+m.start()-20,70)
  for m in re.finditer(b'\x8b\x4e[\x40-\x70]',b[lo:hi]):
   a=lo+m.start()
   if b'\xc7\x46' in b[a:a+25] and b'\xe8' in b[a:a+15]:print('MEMBER RELEASE',hex(a));dis(b,base,bits,a,35)
 for name in names:
  pos=b.find(name);td=pos-2*w;needle=struct.pack('<I',td+(base if bits==32 else 0));cols=[]
  for m in re.finditer(re.escape(needle),b):
   col=m.start()-12
   if int.from_bytes(b[col:col+4],'little')!=(1 if bits==64 else 0):continue
   for ref in re.finditer(re.escape((base+col).to_bytes(w,'little')),b):
    vt=ref.start()+w
    if vt%w:continue
    cols.append(vt)
  print(name,'TABLES',[hex(x) for x in cols])
  for vt in cols:
   if False and b'CSpriteUber_' in name:
    if bits==64:
     # Locate native model-load virtual from the proven 12164 implementation.
     if pid==24984:
      init_slot=b.find((base+0x10e5a10).to_bytes(w,'little'),vt,vt+0x200)-vt
      print('INIT SLOT',hex(init_slot))
     slot=init_slot
    else:slot=init_slot//2
    fn=ptr(vt+slot)-base;print('MODEL INIT',hex(fn));dis(b,base,bits,fn,210)
   for slot in [0,2*w]:
    fn=ptr(vt+slot)-base
    if 0<fn<len(b):print('slot',slot,hex(fn));dis(b,base,bits,fn,60)
   if bits==32 and b'CSpriteUber_' in name:
    fn=ptr(vt)-base
    d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32);d.detail=True
    call=next(i for i in d.disasm(b[fn:fn+40],base+fn) if i.mnemonic=='call')
    print('SPRITE DESTRUCTOR');dis(b,base,bits,call.operands[0].imm-base,550)
   if bits==32 and name==b'.?AUSTRINGREF@@':
    for m in re.finditer(re.escape((base+vt).to_bytes(4,'little')),b[:0x1700000]):
     print('STRINGREF CONSTRUCTION',hex(m.start()));dis(b,base,bits,m.start()-2,100)
