import pathlib,re,struct,pefile,json,capstone
root=pathlib.Path(__file__).resolve().parents[1]
def rtti(b,base,name):
 result=[]
 for m in re.finditer(re.escape(('.?AV'+name+'@@\0').encode()),b):
  descriptor=m.start()-8
  for r in re.finditer(re.escape(struct.pack('<I',base+descriptor)),b):
   locator=r.start()-12
   if locator<0:continue
   sig,offset,cd=struct.unpack_from('<III',b,locator)
   if sig or offset>4096 or cd:continue
   for v in re.finditer(re.escape(struct.pack('<I',base+locator)),b):
    pos=v.start()+4
    entries=[]
    for i in range(160):
     value=struct.unpack_from('<I',b,pos+i*4)[0]-base
     if value<0 or value>=len(b):break
     entries.append(value)
    if entries:result.append({'rva':pos,'offset':offset,'entries':entries})
 return result
p=pefile.PE(str(root.parent/'WHT-1.29/runtime-test/Warcraft3-129/war3.exe'),fast_load=True)
images={'129':(p.get_memory_mapped_image(),p.OPTIONAL_HEADER.ImageBase),'12164':((root/'analysis/memory-14740.bin').read_bytes(),0x1d0000),'12173':((root/'analysis/memory-23884.bin').read_bytes(),0x270000)}
result={};names=['CSimpleFrame','CSimpleFontString','CSimpleTexture','CEditBox','CTextFrame','CGameUI','CCamera','CCameraWar3','CGameCamera','CChatDisplay','CChatEditBar','CCommandButton','CPortraitButton']
for version,(b,base) in images.items():
 result[version]={}
 for name in names:
  tables=rtti(b,base,name);result[version][name]=tables
  print(version,name,[(hex(t['rva']),t['offset'],[(hex(i*4),hex(x)) for i,x in enumerate(t['entries'])][:40]) for t in tables[:2]])
(root/'analysis/rtti131.json').write_text(json.dumps(result,indent=2))
