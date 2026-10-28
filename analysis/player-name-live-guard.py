from pathlib import Path
import ctypes as c,ctypes.wintypes as w,json,struct,sys
r=Path(__file__).resolve().parents[1];pid=int(sys.argv[1]);live=int(sys.argv[2],0);build=int(sys.argv[3]);bits=int(sys.argv[4]);p=next(p for p in json.loads((r/'analysis/player-name-evidence.json').read_text())['profiles'] if p['build']==build and p['bits']==bits);b=(r/f"analysis/memory-{p['capture']}.bin").read_bytes();pe=struct.unpack_from('<I',b,60)[0];base=struct.unpack_from('<I' if bits==32 else '<Q',b,pe+24+(28 if bits==32 else 24))[0];rel=[]
if bits==32:
 a,size=struct.unpack_from('<II',b,pe+24+96+40);end=a+size
 while a<end:
  page,n=struct.unpack_from('<II',b,a)
  if n<8:break
  for q in range(a+8,a+n,2):
   v=struct.unpack_from('<H',b,q)[0]
   if v>>12==3:rel.append(page+(v&4095))
  a+=n
k=c.WinDLL('kernel32',use_last_error=True);k.OpenProcess.argtypes=[w.DWORD,w.BOOL,w.DWORD];k.OpenProcess.restype=w.HANDLE;k.ReadProcessMemory.argtypes=[w.HANDLE,c.c_void_p,c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)];k.CloseHandle.argtypes=[w.HANDLE];h=k.OpenProcess(0x410,False,pid);checks=[]
if not h:raise c.WinError(c.get_last_error())
for a,n in p['blocks']:
 expected=bytearray(b[a:a+n])
 for rv in rel:
  if a<=rv and rv+4<=a+n:struct.pack_into('<I',expected,rv-a,(struct.unpack_from('<I',expected,rv-a)[0]+live-base)&0xffffffff)
 out=c.create_string_buffer(n);got=c.c_size_t();ok=k.ReadProcessMemory(h,live+a,out,n,c.byref(got));checks.append({'rva':hex(a),'size':n,'matches':bool(ok and got.value==n and out.raw==expected)})
k.CloseHandle(h);result={'pid':pid,'build':build,'bits':bits,'read_only':True,'matches':sum(x['matches'] for x in checks),'total':len(checks),'blocks':checks};(r/f'analysis/player-name-live-guard-{build}-{bits}.json').write_text(json.dumps(result,indent=2));print({k:v for k,v in result.items() if k!='blocks'})
