from pathlib import Path
import json,sys
root=Path(__file__).resolve().parents[1];pid=int(sys.argv[1]);r=root/'analysis';row=json.loads((r/f'player-name-live-{pid}.json').read_text());base=int(row['base'],16);width=row.get('bits',64)//8;names={h['address']:h['name'] for h in row['hits']};out={}
for h in row['hits']:
 data=bytes.fromhex(h['hex']);off=int(h['address'],16)-int(h['near_address'],16)-4*width
 if off>=0:
  ptr=int.from_bytes(data[off:off+width],'little')-base
  if 0x800000<ptr<0xb00000:out.setdefault(h['name'],set()).add(hex(ptr))
refpath=r/f'player-name-live-name-refs-{pid}.json'
for refs in json.loads(refpath.read_text()) if refpath.exists() else []:
 for h in refs['hits']:
  data=bytes.fromhex(h['hex']);off=int(h['address'],16)-int(h['near_address'],16)
  for delta in [-width,width]:
   ptr=int.from_bytes(data[off+delta:off+delta+width],'little')-base
   if 0x800000<ptr<0xb00000:out.setdefault(names[h['name']],set()).add(hex(ptr))
result={'pid':pid,'bits':width*8,'read_only':True,'bindings':{k:sorted(v) for k,v in out.items()}};(r/f'player-name-resolved-{pid}.json').write_text(json.dumps(result,indent=2));print(result)
