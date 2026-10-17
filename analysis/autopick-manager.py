exec(open('WHT-1.31/analysis/autopick-live-details.py').read().split("world=u(")[0])
world=u(0x4b0000+0x1aafa78)^0x3393cddb
for a in range(world,world+0x1000,4):
 v=u(a)
 if 0x22289000<=v<0x2228a000:print('WORLDREF',hex(a-world),hex(v))
for a in range(0x22289000,0x22289ed0,4):
 v=u(a)
 if 0x4b0000<v<0x4b0000+0x1c00000: print('VT?',hex(a),hex(v-0x4b0000))
print('PREFIX',read(0x22289800,128).hex())
for v in [0x22289808,0x22289800,0x2228980c]:print('REFERENCES',hex(v),[hex(x) for x in refsTo(v)])
