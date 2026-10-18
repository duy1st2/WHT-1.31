
import ctypes as c,struct,json,sys,time
from pathlib import Path
pid=int(sys.argv[1]);base=int(sys.argv[2]);build=int(sys.argv[3]);bits=int(sys.argv[4]);size=bits//8
k=c.WinDLL('kernel32',use_last_error=True);k.OpenProcess.restype=c.c_void_p;h=k.OpenProcess(0x410,False,pid)
if not h:raise OSError(c.get_last_error())
k.ReadProcessMemory.argtypes=[c.c_void_p,c.c_void_p,c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)]
def rd(a,n):
 b=c.create_string_buffer(n);r=c.c_size_t()
 if not k.ReadProcessMemory(h,a,b,n,c.byref(r)):raise OSError(hex(a))
 return b.raw
def ptr(a):return int.from_bytes(rd(a,size),'little')
def u32(a):return int.from_bytes(rd(a,4),'little')
def text(font):
 p=ptr(font+(0x108 if size==8 else 0xa8));out=b''
 if not p:return ''
 for n in range(128):
  ch=rd(p+n,1)
  if ch==bytes([0]):break
  out+=ch
 return out.decode(errors='replace')
if size==4:
 x,y,constant,uiVT,consoleVT,portraitVT,infoVT,barVT,fontVT=(0x1b1689e,0x1b2be78,-0x7d7a2d08,0x18240a0,0x182d714,0x182c178,0x1827518,0x182e5fc,0x1853b0c) if build==12164 else (0x1acda3e,0x1ae3e70,0x4413917e,0x17d8f80,0x17e25f4,0x17e1058,0x17dc3f8,0x17e34dc,0x1808a2c)
 ui=((u32(base+x)+constant)&0xffffffff)^u32(base+y)
else:
 if build==12164:
  x=0xc44871+0x14f2d6a;y=0xc44882+0x1511106;ui=((ptr(base+x)+0xe0c4d0d5849eb5f7)&0xffffffffffffffff)^ptr(base+y)
  uiVT,consoleVT,portraitVT,infoVT,barVT,fontVT=0x1c00ec0,0x1c13208,0x1c10810,0x1c084e8,0x1c14f08,0x1c58298
 else:
  x=0xc8aa71+0x14da251;y=0xc8aa82+0x14f7f06;ui=ptr(base+x)^0xde1a7bc622efe8e1^ptr(base+y)
  uiVT,consoleVT,portraitVT,infoVT,barVT,fontVT=0x1c2fd78,0x1c420c8,0x1c3f6d0,0x1c373a8,0x1c43dc8,0x1c871a8
assert ptr(ui)==base+uiVT or ptr(ptr(ui)-size)==ptr(base+uiVT-size),'not in active game UI'
components={}
for off in range(size,0x7e8 if size==8 else 0x4b0 if build==12164 else 0x4d4,size):
 obj=ptr(ui+off)
 try:vt=ptr(obj)-base
 except:continue
 if vt in [consoleVT,portraitVT,infoVT]:components.setdefault(vt,[]).append((off,obj))
consoles=components[consoleVT];console=consoles[-1][1];portrait=components[portraitVT][0][1]
out={'pid':pid,'build':build,'architecture':bits,'consoles':[dict(offset=hex(o),visibility=rd(f+(0xf0 if size==8 else 0x9c),2).hex()) for o,f in consoles]}
for name,off in [('HP',0x3c8 if size==8 else 0x24c),('MP',0x3d0 if size==8 else 0x250)]:
 try:out[name]=text(ptr(portrait+off))
 except:out[name]='unavailable'
manager=ptr(console+(0xb8 if size==8 else 0x74));node=ptr(manager+(0x2b0 if size==8 else 0x18c));seen=set();bars=[];col=ptr(base+barVT-size)
while node and not node&1 and node not in seen and len(seen)<32768:
 seen.add(node)
 try:
  f=ptr(node+(0x10 if size==8 else 8));vt=ptr(f);unit=ptr(f+(0x210 if size==8 else 0x154))
  if vt and unit and ptr(vt-size)==col and u32(f+(0x1e8 if size==8 else 0x134))==5:
   texture=ptr(f+(0x1e0 if size==8 else 0x130));bars.append({'bar':f,'unit':unit,'owner':u32(unit+(0x198 if size==8 else 0x164)),'textureABGR':rd(texture+(0xb8 if size==8 else 0x74),4).hex(),'visibility':rd(f+(0xf0 if size==8 else 0x9c),2).hex(),'unitFlags':hex(u32(unit+(0x38 if size==8 else 0x20))),'gated':vt!=base+barVT,'texture':texture})
 except:pass
 node=ptr(node+(8 if size==8 else 4))
out['frames']=len(seen);out['boundBars']=bars[:20]
if bars:
 target=next((b for b in bars if b['owner']<24 and b['visibility']=='0101'),bars[0]);samples=[]
 for i in range(20):
  samples.append(rd(target['texture']+(0xb8 if size==8 else 0x74),4).hex());time.sleep(.02)
 out['colorSamples']=samples
print(json.dumps({key:value for key,value in out.items() if key!='boundBars'},indent=2));Path(f'WHT-1.31/analysis/graphics-repair-live-{build}-{bits}.json').write_text(json.dumps(out,indent=2))
