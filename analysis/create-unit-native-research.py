from port3_research import *
old,base=pe(root.parent/'WHT-1.29/runtime-test/Warcraft3-129/war3.exe');print('OLD CREATE WRAPPER');dis(old,base,32,0x8ee00,230)
print('OLD CREATOR');dis(old,base,32,0x263ca0,300)
for pid in [8740,32116]:
 b,base,bits=image(pid);print('CANDIDATES',pid)
 for m in re.finditer(b'\xff\x75\x14\xff\x75\x10\xff\x75\x0c[\x50-\x57]\xe8',b):
  a=m.start()
  if 0x880000<a<0x970000:print('CREATE?',hex(a));dis(b,base,bits,a-25,90)
for row in json.loads((root/'analysis/binary131.json').read_text(encoding='utf-8')):
 print(row['path'],json.dumps(row['natives'].get('CreateUnit'),indent=2))
