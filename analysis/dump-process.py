import ctypes as c, ctypes.wintypes as w, pathlib, sys, struct, json
k=c.WinDLL('kernel32',use_last_error=True)
k.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD]; k.OpenProcess.restype=w.HANDLE
k.ReadProcessMemory.argtypes=[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)]
k.ReadProcessMemory.restype=w.BOOL
pid=int(sys.argv[1]); base=int(sys.argv[2],0) if len(sys.argv)>2 else 0x400000
h=k.OpenProcess(0x410,False,pid)
if not h:raise c.WinError(c.get_last_error())
def read(addr,size):
 b=c.create_string_buffer(size); count=c.c_size_t();k.ReadProcessMemory(h,addr,b,size,c.byref(count));return b.raw[:count.value]
header=read(base,4096)
if header[:2]!=b'MZ':raise RuntimeError('No PE at base')
pe=struct.unpack_from('<I',header,0x3c)[0];size=struct.unpack_from('<I',header,pe+24+56)[0]
out=bytearray(size)
for pos in range(0,size,4096):
 b=read(base+pos,min(4096,size-pos));out[pos:pos+len(b)]=b
dest=pathlib.Path(__file__).parent/f'memory-{pid}.bin';dest.write_bytes(out)
print(json.dumps({'pid':pid,'base':hex(base),'size':size,'output':str(dest),'camera_string':out.find(b'SetCameraField\0')}))
k.CloseHandle.argtypes=[w.HANDLE];k.CloseHandle(h)
