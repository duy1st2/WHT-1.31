from port3_research import *
b,base,bits=image(24984)
print('BASE',hex(base))
pattern=bytes.fromhex('488b4158');a=0;hits=[]
while True:
 a=b.find(pattern,a)
 if a<0:break
 hits.append(a);a+=1
print('HITS',[hex(x) for x in hits])
for x in hits:
 if 0xf00000<x<0x1100000:dis(b,base,bits,x,80)
v=json.loads((Path(__file__).parent/'model-live-4016.json').read_text(encoding='utf-8'))['selected'][0]
for child in v['children']:
 print('CHILD',child['offset'],child['address'],child['head'])
 table=int.from_bytes(bytes.fromhex(child['head'])[:8],'little')-base
 if 0<table<len(b):
  print('VT',hex(table))
  for o in range(0,0x60,8):
   fn=struct.unpack_from('<Q',b,table+o)[0]-base
   if 0<fn<len(b):print('SLOT',hex(o));dis(b,base,bits,fn,45)
print('MODELVT')
for o in range(0,0x100,8):
 fn=struct.unpack_from('<Q',b,0x1c720a0+o)[0]-base
 if 0<fn<len(b):print('SLOT',hex(o));dis(b,base,bits,fn,28)
print('MODEL RESOURCE REFERENCES')
dis(b,base,bits,0x10e5960,340)
for match in re.finditer(b'\x48[\x8b\x89][\x80-\xbf]\x98\x00\x00\x00',b[0x10d0000:0x1100000]):
 a=0x10d0000+match.start();dis(b,base,bits,a,65)
