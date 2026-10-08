# Preserve Miles' decorated export names exactly (MSVC aliases strip @ suffixes).
import json,struct,sys
from pathlib import Path
path=Path(sys.argv[1]);names=json.loads(Path(sys.argv[2]).read_text());d=bytearray(path.read_bytes())
u16=lambda o:struct.unpack_from('<H',d,o)[0]
u32=lambda o:struct.unpack_from('<I',d,o)[0]
pe=u32(0x3c);opt=pe+24;sections=[]
for i in range(u16(pe+6)):
 o=opt+u16(pe+20)+40*i
 sections.append((u32(o+12),u32(o+16),u32(o+20)))
def offset(rva):
 for va,size,raw in sections:
  if va<=rva<va+size:return raw+rva-va
 raise ValueError('Invalid RVA')
def rva(offset):
 for va,size,raw in sections:
  if raw<=offset<raw+size:return va+offset-raw
 raise ValueError('Invalid offset')
export=offset(u32(opt+96));base=u32(export+16);count=u32(export+24)
assert count==len(names)==353
name_table=offset(u32(export+32));ord_table=offset(u32(export+36))
arena=None
for i in range(u16(pe+6)):
 o=opt+u16(pe+20)+40*i
 if d[o:o+8].rstrip(b'\0')==b'.whtname':arena=u32(o+20);assert u32(o+16)>=32768
assert arena is not None
cursor=arena
for i,(ordinal,name) in enumerate(sorted(names.items(),key=lambda x:x[1])):
 data=name.encode()+b'\0';assert cursor+len(data)<arena+32768
 d[cursor:cursor+len(data)]=data
 struct.pack_into('<I',d,name_table+4*i,rva(cursor))
 struct.pack_into('<H',d,ord_table+2*i,int(ordinal)-base)
 cursor+=len(data)
path.write_bytes(d)
print('Preserved 353 original Miles export names/ordinals')
