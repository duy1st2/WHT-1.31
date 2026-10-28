from port3_research import *
e=json.loads((root/'analysis/mute-chat-native-evidence.json').read_text(encoding='utf-8'));report=[]
for p in e['profiles']:
 if p['bits']!=64:continue
 b,base,bits=image(p['pid']);pe=struct.unpack_from('<I',b,60)[0];rr,size=struct.unpack_from('<II',b,pe+24+112+40);a=rr;found=[]
 while a<rr+size:
  page,n=struct.unpack_from('<II',b,a)
  if n<8:break
  for q in range(a+8,a+n,2):
   v=struct.unpack_from('<H',b,q)[0];r=page+(v&4095)
   if v>>12==10 and any(s<=r<s+n for s,n in p['blocks']):found.append(r)
  a+=n
 assert not found,found
 report.append({'build':p['build'],'bits':64,'guarded_DIR64_relocations':found})
(root/'analysis/mute-chat-relocations.json').write_text(json.dumps(report,indent=2),encoding='utf-8');print(report)
