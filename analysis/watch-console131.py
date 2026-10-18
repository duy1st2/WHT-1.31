
import ctypes as c,struct,time,json
from pathlib import Path
pid=11552;base=9175040
k=c.WinDLL('kernel32',use_last_error=True);user=c.WinDLL('user32',use_last_error=True)
k.OpenProcess.restype=c.c_void_p;k.OpenThread.restype=c.c_void_p
k.ReadProcessMemory.argtypes=[c.c_void_p,c.c_void_p,c.c_void_p,c.c_size_t,c.POINTER(c.c_size_t)]
h=k.OpenProcess(0x410,False,pid)
def rd(a,n):
 b=c.create_string_buffer(n);r=c.c_size_t()
 if not k.ReadProcessMemory(h,a,b,n,c.byref(r)):return b''
 return b.raw
def u(a):return struct.unpack('<I',rd(a,4))[0]
ui=((u(base+0x1acda3e)+0x4413917e)&0xffffffff)^u(base+0x1ae3e70);frame=u(ui+0x498);address=frame+0x9c
main=0
@c.WINFUNCTYPE(c.c_bool,c.c_void_p,c.c_ssize_t)
def enum(hwnd,param):
 global main
 p=c.c_ulong();tid=user.GetWindowThreadProcessId(hwnd,c.byref(p))
 if p.value==pid and user.IsWindowVisible(hwnd):main=tid;return False
 return True
user.EnumWindows(enum,0)
class Float(c.Structure):_fields_=[('a',c.c_uint32*7),('b',c.c_ubyte*80),('cr',c.c_uint32)]
class Context(c.Structure):
 _fields_=[('flags',c.c_uint32),('dr0',c.c_uint32),('dr1',c.c_uint32),('dr2',c.c_uint32),('dr3',c.c_uint32),('dr6',c.c_uint32),('dr7',c.c_uint32),('float',Float),('gs',c.c_uint32),('fs',c.c_uint32),('es',c.c_uint32),('ds',c.c_uint32),('edi',c.c_uint32),('esi',c.c_uint32),('ebx',c.c_uint32),('edx',c.c_uint32),('ecx',c.c_uint32),('eax',c.c_uint32),('ebp',c.c_uint32),('eip',c.c_uint32),('cs',c.c_uint32),('eflags',c.c_uint32),('esp',c.c_uint32),('ss',c.c_uint32),('extended',c.c_ubyte*512)]
class Event(c.Structure):_fields_=[('code',c.c_uint32),('pid',c.c_uint32),('tid',c.c_uint32),('pad',c.c_uint32),('data',c.c_ubyte*160)]
k.Wow64GetThreadContext.argtypes=[c.c_void_p,c.POINTER(Context)];k.Wow64SetThreadContext.argtypes=[c.c_void_p,c.POINTER(Context)]
thread=k.OpenThread(0x1fffff,False,main);hits=[];attached=False;armed=False
try:
 if not thread or not k.DebugActiveProcess(pid):raise OSError(c.get_last_error())
 attached=True;k.DebugSetProcessKillOnExit(False);deadline=time.monotonic()+10
 while time.monotonic()<deadline and len(hits)<4:
  event=Event()
  if not k.WaitForDebugEvent(c.byref(event),200):continue
  status=0x10002
  if event.code==1:
   exception=struct.unpack_from('<I',bytes(event.data))[0]
   if exception==0x80000003 and not armed:
    ctx=Context();ctx.flags=0x10011
    if not k.Wow64GetThreadContext(thread,c.byref(ctx)):raise OSError(c.get_last_error())
    ctx.dr0=address;ctx.dr7=(ctx.dr7&~0xf000f)|0x10001;ctx.dr6=0
    if not k.Wow64SetThreadContext(thread,c.byref(ctx)):raise OSError(c.get_last_error())
    armed=True
   elif exception==0x80000004 and event.tid==main:
    ctx=Context();ctx.flags=0x10011;k.Wow64GetThreadContext(thread,c.byref(ctx))
    hits.append(dict(eip=hex(ctx.eip),rva=hex(ctx.eip-base),ecx=hex(ctx.ecx),esi=hex(ctx.esi),bytes=rd(ctx.eip-12,32).hex(),stack=rd(ctx.esp,64).hex(),frame=hex(frame),visibility=rd(frame+0x9c,2).hex()))
   elif exception!=0x80000003:status=0x80010001
  k.ContinueDebugEvent(event.pid,event.tid,status)
finally:
 if attached:
  k.SuspendThread(c.c_void_p(thread));ctx=Context();ctx.flags=0x10010
  if k.Wow64GetThreadContext(thread,c.byref(ctx)):
   ctx.dr0=0;ctx.dr6=0;ctx.dr7&=~0xf000f;k.Wow64SetThreadContext(thread,c.byref(ctx))
  k.ResumeThread(c.c_void_p(thread));k.DebugActiveProcessStop(pid)
 k.CloseHandle(c.c_void_p(thread));k.CloseHandle(c.c_void_p(h))
print(json.dumps(hits,indent=2));Path('WHT-1.31/analysis/console-write-trace131.json').write_text(json.dumps(hits,indent=2))
