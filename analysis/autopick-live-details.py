exec(open('WHT-1.31/analysis/scan-autopick-items.py').read().split('pathlib.Path')[0])
def u(a):
 d=read(a,4);return struct.unpack('<I',d)[0] if len(d)==4 else 0
def refsTo(v):
 out=[];needle=struct.pack('<I',v)
 for start,data in blocks:
  p=0
  while True:
   p=data.find(needle,p)
   if p<0:break
   if (start+p)%4==0:out.append(start+p)
   p+=4
 return out
world=u(0x4b0000+0x1aafa78)^0x3393cddb;print('WORLD',hex(world));
print('LIST',read(0x2ace96f0,80).hex())
for v in [0x2ace9700,0x2ace9708,0x2ace9704]:
 r=refsTo(v);print('REF',hex(v),[hex(x) for x in r]);
 for a in r:print(hex(a),read(a-16,48).hex(),'worldOff',hex(a-world))
for x in items:print('ITEM',hex(x),'type',read(x+0x30,4)[::-1], 'id',read(x+12,8).hex(),'flags',hex(u(x+0x20)),'prefix',read(x+0x140,0x90).hex())
