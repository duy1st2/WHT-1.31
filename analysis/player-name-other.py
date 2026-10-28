from port3_research import *
for pid,setter in [(32116,0x8e91c0),(24984,0xa32b20),(27336,0xa7b460)]:
 b,base,bits=image(pid);print('PID',pid);dis(b,base,bits,setter,80)
