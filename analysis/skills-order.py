from port3_research import *
for pid,constant in [(8740,0x182f564),(32116,0x17e4444)]:
 b,base,bits=image(pid);print(pid)
 for m in re.finditer(re.escape(struct.pack('<I',base+constant)),b):
  a=m.start();print('ref',hex(a));dis(b,base,bits,max(0,a-20),100)
