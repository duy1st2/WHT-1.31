import ctypes as c, ctypes.wintypes as w, struct, json, sys
from pathlib import Path
k=c.WinDLL('kernel32',use_last_error=True)
k.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD];k.OpenProcess.restype=w.HANDLE
k.ReadProcessMemory.argtypes=[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)]
k.VirtualQueryEx.argtypes=[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t]
k.CloseHandle.argtypes=[w.HANDLE]
class M(c.Structure):
 _fields_=[('base',c.c_void_p),('allocation',c.c_void_p),('ap',w.DWORD),('size',c.c_size_t),('state',w.DWORD),('protect',w.DWORD),('type',w.DWORD)]
pid=int(sys.argv[1]);base=int(sys.argv[2],0);h=k.OpenProcess(0x410,False,pid)
if not h:raise c.WinError(c.get_last_error())
def read(a,n):
 b=c.create_string_buffer(n);got=c.c_size_t();k.ReadProcessMemory(h,a,b,n,c.byref(got));return b.raw[:got.value]
def ptr(a):
 b=read(a,8);return int.from_bytes(b,'little') if len(b)==8 else 0
portraits=[];a=0;needle=struct.pack('<Q',base+0x1c10810)
while a<0x7fffffff0000:
 m=M()
 if not k.VirtualQueryEx(h,a,c.byref(m),c.sizeof(m)):break
 start=m.base or 0;a=start+m.size
 if m.state!=0x1000 or m.protect&0x101 or m.type!=0x20000:continue
 for off in range(0,m.size,4*1024*1024):
  data=read(start+off,min(m.size-off,4*1024*1024));pos=0
  while True:
   pos=data.find(needle,pos)
   if pos<0:break
   at=start+off+pos;pos+=8
   if at%8:continue
   unit=ptr(at+0x3b8)
   if ptr(unit)!=base+0x1c16140:continue
   model=ptr(unit+0x48);paths=[]
   # Exploratory pointer graph only; findings are not an enabled reader.
   for o in range(0,0x400,8):
    target=ptr(model+o)
    if target<0x10000:continue
    block=read(target,0x200)
    for p in range(0,len(block)-7,8):
     s=read(int.from_bytes(block[p:p+8],'little'),260).split(b'\0',1)[0]
     if 4<len(s)<260 and all(32<=x<127 for x in s) and (b'.mdx' in s.lower() or b'.mdl' in s.lower()):
      paths.append({'model_offset':hex(o),'child_offset':hex(p),'path':s.decode('ascii')})
    s=block.split(b'\0',1)[0]
    if 4<len(s)<260 and all(32<=x<127 for x in s) and (b'.mdx' in s.lower() or b'.mdl' in s.lower()):paths.append({'model_offset':hex(o),'path':s.decode('ascii')})
   children=[{'offset':hex(o),'address':hex(ptr(model+o)),'head':read(ptr(model+o),0xc0).hex()} for o in [0x58,0x98,0xa0,0xa8]]
   name=ptr(model+0x98);text=ptr(name+0x38)
   exact={'model_class_matches':ptr(model)==base+0x1c720a0,'stringref_class_matches':ptr(name)==base+0x1c736d8,'name':hex(name),'text':hex(text),'path':read(text,512).split(b'\0',1)[0].decode('ascii',errors='replace'),'selection_still_matches':ptr(at+0x3b8)==unit}
   portraits.append({'portrait':hex(at),'unit':hex(unit),'model':hex(model),'verified_chain':exact,'model_head':read(model,0x200).hex(),'children':children,'paths':paths})
k.CloseHandle(h)
result={'pid':pid,'base':hex(base),'build':12164,'architecture':'x64','read_only':True,'selected':portraits}
Path(__file__).with_name(f'model-live-{pid}.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
print(json.dumps(result,indent=2))
