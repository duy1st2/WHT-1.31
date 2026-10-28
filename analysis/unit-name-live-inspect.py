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
   raw=read(unit+0x5c,260);name=raw.split(b'\0',1)[0]
   portraits.append({'portrait':hex(at),'unit':hex(unit),'name':name.decode('utf-8',errors='replace'),'terminated':b'\0' in raw,'selection_still_matches':ptr(at+0x3b8)==unit})
k.CloseHandle(h)
result={'pid':pid,'base':hex(base),'build':12164,'architecture':'x64','read_only':True,'selected':portraits}
Path(__file__).with_name(f'unit-name-live-{pid}.json').write_text(json.dumps(result,indent=2),encoding='utf-8')
print(json.dumps(result,indent=2))
