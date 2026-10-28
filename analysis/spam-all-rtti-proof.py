from port3_research import *
p=root/'analysis/spam-all-evidence.json';e=json.loads(p.read_text());proof=[]
for pid,ctor in [(8740,0x9c9f00),(32116,0x98aa90),(24984,0xaec0c0),(27336,0xb34bc0)]:
 b,base,bits=image(pid);w=bits//8;at=b.find(b'.?AVCNetCommandAllyPing@@\0');assert at>=0;td=at-2*w;tables=[]
 for m in re.finditer(re.escape(struct.pack('<I',base+td if bits==32 else td)),b):
  col=m.start()-12
  if col<0 or struct.unpack_from('<I',b,col)[0]!=(1 if bits==64 else 0):continue
  tables += [x.start()+w for x in re.finditer(re.escape(int(base+col).to_bytes(w,'little')),b)]
 if bits==32:table=struct.unpack_from('<I',b,ctor+0x4a)[0]-base
 else:
  a=ctor+0x2f;assert b[a:a+3]==b'\x48\x8d\x05';table=a+7+struct.unpack_from('<i',b,a+3)[0]
 assert table in tables,(pid,hex(table),tables)
 proof.append({'capture':pid,'class':'CNetCommandAllyPing','producer_vtable':hex(table),'rtti_confirmed':True})
e['rtti_proof']=proof;p.write_text(json.dumps(e,indent=2));print(json.dumps(proof,indent=2))
