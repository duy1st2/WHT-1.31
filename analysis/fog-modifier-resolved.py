from port3_research import *
r=json.loads((root/'analysis/fog-modifier-live-1416.json').read_text());names={h['address']:h['name'] for h in r['hits']};row=json.loads((root/'analysis/fog-modifier-live-name-refs.json').read_text())[0];lb=int(row['base'],16);b,base,bits=image(24984)
for h in row['hits']:
 data=bytes.fromhex(h['hex']);at=int(h['address'],16)-32-int(h['near_address'],16);fn=int.from_bytes(data[at:at+8],'little')-lb
 if 0x900000<fn<0xb00000:
  print(names[h['name']],hex(fn));dis(b,base,bits,fn,430)
