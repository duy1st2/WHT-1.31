from pathlib import Path
import struct
mask=0xffffffff;crypt=[0]*0x500;seed=0x100001
for i in range(0x100):
 for j in range(5):
  seed=(seed*125+3)%0x2aaaab;hi=(seed&0xffff)<<16;seed=(seed*125+3)%0x2aaaab;crypt[i+j*0x100]=hi|(seed&0xffff)
def hash(s,kind):
 a=0x7fed7fed;b=0xeeeeeeee
 for ch in s.upper().encode():a=(crypt[kind*0x100+ch]^(a+b))&mask;b=(ch+a+b+(b<<5)+3)&mask
 return a
def transform(data,key,encrypt=False):
 out=[];seed=0xeeeeeeee
 for value in struct.unpack('<'+'I'*(len(data)//4),data):
  seed=(seed+crypt[0x400+(key&255)])&mask;x=value^((key+seed)&mask);out.append(x);plain=value if encrypt else x
  key=(((~key<<21)+0x11111111)|(key>>11))&mask;seed=(plain+seed+(seed<<5)+3)&mask
 return struct.pack('<'+'I'*len(out),*out)
p=Path('WHT-1.31/runtime-test/Warcraft3-1.31.1.12164/WHT-autopick-analysis.w3x');d=bytearray(p.read_bytes());base=d.find(b'MPQ\x1a');hp,bp,hn,bn=struct.unpack_from('<IIII',d,base+16)
hashes=transform(d[base+hp:base+hp+hn*16],hash('(hash table)',3));blocks=bytearray(transform(d[base+bp:base+bp+bn*16],hash('(block table)',3)))
index=None
for name in ['war3map.j','scripts\\war3map.j']:
 for i in range(hn):
  a,b,loc,platform,bi=struct.unpack_from('<IIHHI',hashes,i*16)
  if a==hash(name,1) and b==hash(name,2):index=bi;print('script',name,index)
assert index is not None and index<bn
script='''function config takes nothing returns nothing
 local integer i=0
 call SetMapName("WHT Auto Pick fixture")
 call SetMapDescription("Local native verification")
 call SetPlayers(1)
 call SetTeams(1)
 call DefineStartLocation(0,0.0,0.0)
 loop
 exitwhen i>=12
 call SetPlayerController(Player(i),MAP_CONTROL_NONE)
 set i=i+1
 endloop
 call SetPlayerController(Player(0),MAP_CONTROL_USER)
 call SetPlayerStartLocation(Player(0),0)
 call SetPlayerRacePreference(Player(0),RACE_PREF_HUMAN)
 call SetPlayerRaceSelectable(Player(0),false)
endfunction
function main takes nothing returns nothing
 local unit hero
 call SetCameraBounds(-5000.0,-5000.0,5000.0,5000.0,-5000.0,5000.0,5000.0,-5000.0)
 call InitBlizzard()
 set hero=CreateUnit(Player(0),'Hpal',0.0,0.0,0.0)
 call SelectUnit(hero,true)
 call CreateItem('ratf',150.0,0.0)
 call CreateItem('bspd',0.0,150.0)
 call CreateItem('ches',-150.0,0.0)
 call SetCameraPosition(0.0,0.0)
endfunction
'''.encode()
offset=len(d)-base;d+=script;struct.pack_into('<IIII',blocks,index*16,offset,len(script),len(script),0x81000000)
newbp=len(d)-base;d+=transform(bytes(blocks),hash('(block table)',3),True);struct.pack_into('<I',d,base+8,len(d)-base);struct.pack_into('<I',d,base+20,newbp)
out=p.with_name('WHT-autopick-fixture.w3x');out.write_bytes(d);print(out,len(d))
