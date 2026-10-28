import ctypes as c,ctypes.wintypes as w,json,sys
from pathlib import Path
k=c.WinDLL('kernel32',use_last_error=True);k.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD];k.OpenProcess.restype=w.HANDLE;k.ReadProcessMemory.argtypes=[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)]
pid=int(sys.argv[1]);base=int(sys.argv[2],0);h=k.OpenProcess(0x410,False,pid)
def read(a,n):
 b=c.create_string_buffer(n);got=c.c_size_t();k.ReadProcessMemory(h,a,b,n,c.byref(got));return b.raw[:got.value]
def ptr(a):return int.from_bytes(read(a,8),'little')
world=ptr(base+0x20b9d98)^0x6b208c73c00571ee;config=ptr(world+0xe8);table=ptr(config);col=ptr(table-8);td=int.from_bytes(read(col+12,4),'little');name=read(base+td+16,140).split(b'\0',1)[0]
r={'pid':pid,'base':hex(base),'read_only':True,'world':hex(world),'world_table':hex(ptr(world)-base),'config':hex(config),'config_table':hex(table-base),'rtti_name':name.decode(errors='replace'),'flags':hex(int.from_bytes(read(config+0x60,4),'little'))};print(r);Path('WHT-1.31/analysis/map-trade-live-context.json').write_text(json.dumps(r,indent=2))
