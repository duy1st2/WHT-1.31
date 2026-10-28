import ctypes as c,ctypes.wintypes as w,struct,json,sys
from pathlib import Path
k=c.WinDLL('kernel32',use_last_error=True);k.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD];k.OpenProcess.restype=w.HANDLE;k.ReadProcessMemory.argtypes=[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)];k.VirtualQueryEx.argtypes=[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t];k.CloseHandle.argtypes=[w.HANDLE]
class M(c.Structure):_fields_=[('base',c.c_void_p),('allocation',c.c_void_p),('ap',w.DWORD),('size',c.c_size_t),('state',w.DWORD),('protect',w.DWORD),('type',w.DWORD)]
pid=int(sys.argv[1]);base=int(sys.argv[2],0);h=k.OpenProcess(0x410,False,pid)
if not h:raise c.WinError(c.get_last_error())
hits=[];a=0;names=[b'GroupEnumUnitsOfPlayer\0',b'CreateGroup\0',b'FirstOfGroup\0',b'GroupRemoveUnit\0',b'DestroyGroup\0',b'GetUnitName\0',b'IsMapFlagSet\0']
while a<0x7fffffff0000:
 m=M()
 if not k.VirtualQueryEx(h,a,c.byref(m),c.sizeof(m)):break
 start=m.base or 0;a=start+m.size
 if m.state!=0x1000 or m.protect&0x101 or m.type!=0x20000:continue
 for off in range(0,m.size,4*1024*1024):
  n=min(m.size-off,4*1024*1024);buf=c.create_string_buffer(n);got=c.c_size_t();k.ReadProcessMemory(h,start+off,buf,n,c.byref(got));data=buf.raw[:got.value]
  for name in names:
   pos=0
   while True:
    pos=data.find(name,pos)
    if pos<0:break
    begin=max(0,pos-64);hits.append({'name':name.decode().strip('\0'),'address':hex(start+off+pos),'near_address':hex(start+off+begin),'hex':data[begin:pos+96].hex()});pos+=1
k.CloseHandle(h)
dest=Path(__file__).with_name(f'player-units-live-{pid}.json');dest.write_text(json.dumps({'pid':pid,'base':hex(base),'read_only':True,'hits':hits},indent=2),encoding='utf-8');print(dest,len(hits))
