import sys,struct,subprocess
from pathlib import Path
pid,rva,size=int(sys.argv[1]),int(sys.argv[2],0),int(sys.argv[3],0)
root=Path(__file__).parent
with (root/f"memory-{pid}.bin").open("rb") as f:
 head=bytearray(f.read(0x1000));f.seek(rva);code=f.read(size)
pe=struct.unpack_from("<I",head,60)[0];x64=struct.unpack_from("<H",head,pe+24)[0]==0x20b;base=struct.unpack_from("<Q" if x64 else "<I",head,pe+24+(24 if x64 else 28))[0];s=pe+24+struct.unpack_from("<H",head,pe+20)[0];struct.pack_into("<H",head,pe+6,1);struct.pack_into("<8sIIIIIIHHI",head,s,b".text\0\0\0",size,rva,size,0x1000,0,0,0,0,0x60000020);dd=pe+24+(112 if x64 else 96);head[dd:dd+128]=bytes(128);struct.pack_into("<I",head,dd-4,0)
tmp=root/f"disasm-{pid}-{rva:x}.exe";tmp.write_bytes(head+code)
p=subprocess.run([r"C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.51.36231\bin\Hostx64\x64\dumpbin.exe","/disasm:nobytes",str(tmp)],capture_output=True,text=True)
text=root/f"disasm-{pid}-{rva:x}.txt";text.write_text(p.stdout);print(p.stdout)
