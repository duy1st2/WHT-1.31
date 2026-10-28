import ctypes as c,ctypes.wintypes as w,struct,time,json,sys
from pathlib import Path
k=c.WinDLL('kernel32',use_last_error=True);k.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD];k.OpenProcess.restype=w.HANDLE;k.ReadProcessMemory.argtypes=[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)];k.CloseHandle.argtypes=[w.HANDLE]
pid=int(sys.argv[1]);base=int(sys.argv[2],0);h=k.OpenProcess(0x410,False,pid)
if not h:raise c.WinError(c.get_last_error())
def read(a,n):
 b=c.create_string_buffer(n);got=c.c_size_t()
 if not k.ReadProcessMemory(h,a,b,n,c.byref(got)) or got.value!=n:raise RuntimeError('read failed')
 return b.raw
def ptr(a):return struct.unpack('<Q',read(a,8))[0]
rows=[]
try:
 for sample in range(6):
  ui=ptr(base+0xc8aa71+0x14da251)^0xde1a7bc622efe8e1^ptr(base+0xc8aa82+0x14f7f06)
  bar=ptr(ui+0x6d8)
  if ptr(bar)!=base+0x1c418f0:raise RuntimeError('resource bar class mismatch')
  font=ptr(bar+0x218)
  if ptr(font)!=base+0x1c871a8:raise RuntimeError('font class mismatch')
  text=read(ptr(font+0x108),96).split(b'\0')[0].decode('utf-8')
  rows.append({'sample':sample,'fps_text':text});print(text,flush=True)
  if sample<5:time.sleep(1)
finally:k.CloseHandle(h)
Path(__file__).with_name('lag-repair-before-live-fps.json').write_text(json.dumps({'pid':pid,'build':12173,'bits':64,'read_only':True,'mutations':False,'samples':rows},indent=2),encoding='utf-8')
