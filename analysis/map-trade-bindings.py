from port3_research import *
for file in ['native-live-name-scan-20261028.json','native-live-alliance-save-scan-20261028.json','native-live-x64-bindings-20261028.json']:
 for row in json.loads((root/'analysis'/file).read_text(encoding='utf-8')):
  b,base,bits=image(row['pid']);w=bits//8
  seen=set()
  for hit in row['hits']:
   if hit['name'] not in ['SetMapFlag','IsMapFlagSet']:continue
   data=bytes.fromhex(hit['hex']);at=int(hit['near_address'],16)
   for off in range(0,64-w+1):
    if (at+off)%w:continue
    v=int.from_bytes(data[off:off+w],'little')-base
    if 0x1000<v<0x1800000 and v not in seen:
     seen.add(v);print(row['pid'],hit['name'],hex(v));dis(b,base,bits,v,120)
