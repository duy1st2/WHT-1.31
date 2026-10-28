from port3_research import *
for pid,a in [(8740,0x8e7ae0),(32116,0x8b6060),(24984,0x9fddf0)]:
 b,base,bits=image(pid);print('GETTER',pid);dis(b,base,bits,a+120,210)
