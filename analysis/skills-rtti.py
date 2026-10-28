from port3_research import *
for pid in [8740,32116,24984,27336]:
 b,base,bits=image(pid);print(pid)
 for m in re.finditer(rb'\.\?AV[^\x00]{1,100}\x00',b):
  if b'Command' in m[0]: print(hex(m.start()),m[0])
 print('constants',[hex(m.start()) for m in re.finditer(re.escape(struct.pack('<I',0xd0142)),b)])
