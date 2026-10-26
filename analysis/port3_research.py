from pathlib import Path
import struct,sys,re,json
sys.path.insert(0,str(Path(__file__).parent/'python-tools'));import capstone
root=Path(__file__).resolve().parents[1]
def pe(path):
 raw=Path(path).read_bytes();p=struct.unpack_from('<I',raw,60)[0];n=struct.unpack_from('<H',raw,p+6)[0];o=struct.unpack_from('<H',raw,p+20)[0];b=bytearray(struct.unpack_from('<I',raw,p+24+56)[0]);b[:0x400]=raw[:0x400]
 for i in range(n):
  at=p+24+o+i*40;va,rs,off=struct.unpack_from('<III',raw,at+12);b[va:va+rs]=raw[off:off+rs]
 return bytes(b),struct.unpack_from('<I',raw,p+24+28)[0]
def image(pid):
 b=(root/f'analysis/memory-{pid}.bin').read_bytes();p=struct.unpack_from('<I',b,60)[0];bits=32 if struct.unpack_from('<H',b,p+24)[0]==0x10b else 64;base=struct.unpack_from('<I' if bits==32 else '<Q',b,p+24+(28 if bits==32 else 24))[0];return b,base,bits
def dis(b,base,bits,a,n=150):
 d=capstone.Cs(capstone.CS_ARCH_X86,capstone.CS_MODE_32 if bits==32 else capstone.CS_MODE_64);d.detail=True
 for i in d.disasm(b[a:a+n],a+base):print(hex(i.address-base),i.mnemonic,i.op_str)
if __name__=='__main__':
 old,base=pe(root.parent/'WHT-1.29/runtime-test/Warcraft3-129/war3.exe')
 for name,a,n in [('vertex',0x26dca0,180),('select',0x2cc540,220),('menu',0x1893f0,250)]:
  print(name);dis(old,base,32,a,n)
 for pid,t in [(14740,0x183285c),(23884,0x17e773c),(6688,0x1c16140),(29928,0x1c45000)]:
  b,base,bits=image(pid);a=struct.unpack_from('<I' if bits==32 else '<Q',b,t+45*(bits//8))[0]-base;print('NEW VERTEX',pid,hex(a));dis(b,base,bits,a,180)
