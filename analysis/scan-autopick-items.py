import ctypes as c,ctypes.wintypes as w,struct,json,pathlib
k=c.WinDLL('kernel32',use_last_error=True)
k.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD];k.OpenProcess.restype=w.HANDLE
k.ReadProcessMemory.argtypes=[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)];k.ReadProcessMemory.restype=w.BOOL
class MBI(c.Structure):_fields_=[('BaseAddress',c.c_void_p),('AllocationBase',c.c_void_p),('AllocationProtect',w.DWORD),('PartitionId',w.WORD),('RegionSize',c.c_size_t),('State',w.DWORD),('Protect',w.DWORD),('Type',w.DWORD)]
k.VirtualQueryEx.argtypes=[w.HANDLE,c.c_void_p,c.POINTER(MBI),c.c_size_t];k.VirtualQueryEx.restype=c.c_size_t
h=k.OpenProcess(0x410,False,36764)
def read(a,n):
 out=c.create_string_buffer(n);got=c.c_size_t();k.ReadProcessMemory(h,a,out,n,c.byref(got));return out.raw[:got.value]
blocks=[];items=[];a=0;needle=struct.pack('<I',0x4b0000+0x182f8f4)
while a<0x7fff0000:
 m=MBI()
 if not k.VirtualQueryEx(h,a,c.byref(m),c.sizeof(m)):break
 size=m.RegionSize;start=m.BaseAddress or a
 if m.State==0x1000 and not m.Protect&0x101 and m.Protect&0x0c and m.Type==0x20000:
  for off in range(0,size,0x400000):
   data=read(start+off,min(size-off,0x400000));blocks.append((start+off,data));p=0
   while True:
    p=data.find(needle,p)
    if p<0:break
    if (start+off+p)%4==0:items.append(start+off+p)
    p+=4
 a=start+size
items=[x for x in items if read(x+0xc,8)!=b'\xff'*8]
print('items',len(items),[hex(x) for x in items[:20]],'MB',sum(len(x[1]) for x in blocks)//1048576)
refs={}
for obj in items[:100]:
 needle=struct.pack('<I',obj);out=[]
 for start,data in blocks:
  pos=0
  while True:
   pos=data.find(needle,pos)
   if pos<0:break
   if (start+pos)%4==0:out.append(start+pos)
   pos+=4
 refs[hex(obj)]=[hex(x) for x in out[:40]]
pathlib.Path('WHT-1.31/analysis/autopick-live-items.json').write_text(json.dumps(refs,indent=2));print(json.dumps(refs,indent=2)[:3000])

