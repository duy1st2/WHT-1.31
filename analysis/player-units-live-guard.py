from pathlib import Path
import ctypes as c,ctypes.wintypes as w,json,sys,hashlib
root=Path(__file__).resolve().parents[1];pid=int(sys.argv[1]);base=int(sys.argv[2],0)
k=c.WinDLL('kernel32',use_last_error=True);k.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD];k.OpenProcess.restype=w.HANDLE;k.ReadProcessMemory.argtypes=[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)];k.CloseHandle.argtypes=[w.HANDLE]
h=k.OpenProcess(0x410,False,pid)
if not h:raise c.WinError(c.get_last_error())
p=next(p for p in json.loads((root/'analysis/player-abilities-native-evidence.json').read_text(encoding='utf-8'))['profiles'] if p['build']==12164 and p['bits']==64);capture=(root/'analysis/memory-24984.bin').read_bytes();checks=[]
for rva,n in p['blocks']:
 b=c.create_string_buffer(n);got=c.c_size_t();ok=k.ReadProcessMemory(h,base+rva,b,n,c.byref(got));same=bool(ok and got.value==n and b.raw==capture[rva:rva+n]);checks.append({'rva':hex(rva),'length':n,'matches_capture':same})
k.CloseHandle(h)
result={'pid':pid,'base':hex(base),'build':12164,'bits':64,'read_only':True,'game_native_invoked':False,'blocks':checks};(root/'analysis/player-units-live-guard.json').write_text(json.dumps(result,indent=2),encoding='utf-8');print(result)
