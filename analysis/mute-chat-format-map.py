from port3_research import *
old,oldbase=pe(root.parent/'WHT-1.29/runtime-test/Warcraft3-129/war3.exe')
for a in [0xe93c78]:
 s=old[a-oldbase:a-oldbase+100].split(b'\0')[0];print('FORMAT',s)
 for pid in [8740,32116,24984,27336]:
  b,base,bits=image(pid);p=b.find(s+b'\0');print(pid,hex(p),b[p-16:p+len(s)+8]);positions=[m.start() for m in re.finditer(re.escape(s+b'\0'),b)];print('ALL',list(map(hex,positions)))
  positions=[p-2 for p in positions];p=positions[0]
  if bits==32:
   for p in positions:
    for m in re.finditer(re.escape(struct.pack('<I',base+p)),b):print('XREF',hex(m.start()));dis(b,base,bits,m.start()-4,28)
  else:
   for m in re.finditer(b'[\x48\x4c]\x8d[\x05\x0d\x15\x1d\x25\x2d\x35\x3d]',b):
    a=m.start()
    if a+7+struct.unpack_from('<i',b,a+3)[0]==p:print('XREF',hex(a));dis(b,base,bits,a,30)
