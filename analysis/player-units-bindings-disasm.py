from port3_research import *
row=json.loads((root/'analysis/player-units-live-18240.json').read_text());livebase=int(row['base'],16);b,base,bits=image(24984);seen=set()
for hit in row['hits']:
 data=bytes.fromhex(hit['hex']);at=int(hit['near_address'],16)
 for off in range(len(data)-7):
  if (at+off)%8:continue
  a=int.from_bytes(data[off:off+8],'little')-livebase
  if 0x1000<a<0x1800000 and a not in seen:
   seen.add(a);print(hit['name'],hex(a));dis(b,base,bits,a,360)

