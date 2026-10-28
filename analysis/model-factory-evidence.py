from port3_research import *
for pid,a,init in [(8740,0xf1dd10,0xf077b0),(32116,0xedcce0,0xec6790),(24984,0x10ff3a0,0x10e5a10),(27336,0x113be00,0)]:
 b,base,bits=image(pid);print('PID',pid,'FACTORY',hex(a));dis(b,base,bits,a,250)
 if init:print('INIT');dis(b,base,bits,init,180)
