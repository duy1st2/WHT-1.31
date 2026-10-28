import ctypes as c,ctypes.wintypes as w,json,struct
from pathlib import Path
k=c.WinDLL('kernel32',use_last_error=True);k.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD];k.OpenProcess.restype=w.HANDLE;k.ReadProcessMemory.argtypes=[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)];k.VirtualQueryEx.argtypes=[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t]
class M(c.Structure):_fields_=[('base',c.c_void_p),('allocation',c.c_void_p),('ap',w.DWORD),('size',c.c_size_t),('state',w.DWORD),('protect',w.DWORD),('type',w.DWORD)]
previous=json.loads(Path('WHT-1.31/analysis/native-live-x64-bindings-20261028.json').read_text()); names=[];results=[]
for pid,base in [(24984,0x7ff6e1210000),(27336,0x7ff7c0b30000)]:
 names=[struct.pack('<Q',int(hit['address'],16)) for row in previous if row['pid']==pid for hit in row['hits'] if hit['name'] in ['SetPlayerAlliance','GetPlayerAlliance']]; h=k.OpenProcess(0x410,False,pid);a=0;total=0;hits=[]
 while a<0x7fffffff0000:
  m=M()
  if not k.VirtualQueryEx(h,a,c.byref(m),c.sizeof(m)):break
  start=m.base or 0;a=start+m.size
  if m.state!=0x1000 or m.protect&0x101 or m.type!=0x20000:continue
  for off in range(0,m.size,4*1024*1024):
   n=min(m.size-off,4*1024*1024);buf=c.create_string_buffer(n);got=c.c_size_t();k.ReadProcessMemory(h,start+off,buf,n,c.byref(got));data=buf.raw[:got.value];total+=len(data)
   for name in names:
    pos=0
    while True:
     pos=data.find(name,pos)
     if pos<0:break
     at=start+off+pos;chunk=data[max(0,pos-64):pos+128];hits.append({'name':hex(struct.unpack('<Q',name)[0]),'address':hex(at),'region':hex(start),'near_address':hex(start+off+max(0,pos-64)),'hex':chunk.hex()});pos+=1
 results.append({'pid':pid,'base':hex(base),'scanned':total,'hits':hits});print(pid,total,len(hits),[(x['name'],x['address']) for x in hits][:25],flush=True)
Path('WHT-1.31/analysis/native-live-x64-alliance-refs-20261028.json').write_text(json.dumps(results,indent=2))
