from port3_research import *
for pid,constant in [(8740,0x182f564),(32116,0x17e4444)]:
 b,base,bits=image(pid);print(pid)
 for m in re.finditer(re.escape(struct.pack('<I',base+constant)),b[:0x1800000]):
  a=m.start()
  if b'\xff\x90\xc0\x01\x00\x00' in b[a:a+140] or b'\xff\x90\xc4\x01\x00\x00' in b[a:a+140]:
   print('MATCH',hex(a));dis(b,base,bits,a-24,190)
