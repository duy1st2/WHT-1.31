from port3_research import *
r=json.loads((root/'analysis/fog-modifier-live-1416.json').read_text());livebase=int(r['base'],16);b,base,bits=image(24984)
for h in r['hits']:
 data=bytes.fromhex(h['hex']);at=int(h['address'],16)-32-int(h['near_address'],16)
 if at<0 or at+8>len(data):continue
 fn=int.from_bytes(data[at:at+8],'little')-livebase
 if not 0x900000<fn<0xb00000:continue
 print(h['name'],hex(fn));dis(b,base,bits,fn,240)
