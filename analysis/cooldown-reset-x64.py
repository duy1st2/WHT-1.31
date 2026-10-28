from port3_research import *
for pid in [24984,27336]:
 b,base,bits=image(pid);print('PID',pid)
 for m in re.finditer(bytes.fromhex('be010d00'),b[:0x1800000]):
  a=m.start();print('EVENT',hex(a));dis(b,base,bits,a-25,90)
