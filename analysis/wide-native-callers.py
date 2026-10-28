from port3_research import *
b,base,bits=image(8740);targets={0xf47390:'sqrt',0xf47560:'tan'};groups={}
for m in re.finditer(b'\xe8',b[:0x1900000]):
 a=m.start();t=a+5+struct.unpack_from('<i',b,a+1)[0]
 if t in targets:
  start=b.rfind(b'\x55\x8b\xec',max(0,a-3000),a);groups.setdefault(start,[]).append((targets[t],a))
for a,v in groups.items():
 if len({name for name,at in v})==2:print('BOTH',hex(a),[(s,hex(x)) for s,x in v]);dis(b,base,bits,a,max(x for s,x in v)+240-a)
print('TAN',[hex(a) for a,v in groups.items() if any(s=='tan' for s,x in v)])
