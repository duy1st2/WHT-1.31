from port3_research import *
b,base,bits=image(8740)
for a in range(0x1875980,0x1875a30,8):
 vals=struct.unpack_from('<2I',b,a);print(hex(a),[hex(x-base) for x in vals]);n=vals[0]-base
 if 0<n<len(b):print(b[n:n+40].split(b'\0')[0])
for a in [0x1875988,0x1875a10]:
 vals=struct.unpack_from('<2I',b,a);print('FUNCTION',hex(vals[1]-base));dis(b,base,bits,vals[1]-base,100)
