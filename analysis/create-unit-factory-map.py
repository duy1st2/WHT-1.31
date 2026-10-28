from port3_research import *
for pid in [8740,32116,24984,27336]:
 b,base,bits=image(pid);print('PID',pid)
 for m in re.finditer(b'\x02\x05\x00\x00',b[:0x1800000]):
  a=m.start()
  if bits==32 and b[a-5:a]==bytes.fromhex('6aff6a0168'):
   print('FACTORY FLAGS',hex(a));dis(b,base,bits,a-6,105)
  if bits==64 and b[a-5:a].startswith(b'\xc7\x44\x24'):
   print('FLAGS64',hex(a));dis(b,base,bits,a-20,90)
