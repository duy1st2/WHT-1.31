from port3_research import *
for pid in [8740,32116,24984,27336]:
 b,base,bits=image(pid)
 pats=[bytes.fromhex('85d2578bf9'),bytes.fromhex('85d2538bd9') ] if bits==32 else [bytes.fromhex('4885d274'),bytes.fromhex('4885d275')]
 print('PID',pid)
 if bits==32:
  for pat in pats:
   for m in re.finditer(re.escape(pat),b):
    a=m.start()
    if a<0x1000000: print('candidate',hex(a));dis(b,base,bits,a,85)
