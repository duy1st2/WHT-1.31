from port3_research import *
for pid,a,n in [(24984,0x9e0a70,170),(24984,0xd3e4c0,250),(8740,0xbac220,22),(32116,0xb6b690,22)]:
 b,base,bits=image(pid);print(pid,hex(a));dis(b,base,bits,a,n)
for pid,target in [(8740,0xbac220),(32116,0xb6b690)]:
 b,base,bits=image(pid);print('WRAPPERS',pid)
 for m in re.finditer(b'\xe8',b[0x880000:0x970000]):
  q=m.start()+0x880000
  if q+5+struct.unpack_from('<i',b,q+1)[0]==target:print(hex(q));dis(b,base,bits,q-18,47)
