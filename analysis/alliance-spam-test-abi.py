from pathlib import Path
p=Path('WHT-1.31/tests/alliance-spam-tests.cpp');s=p.read_text(encoding='utf-8-sig');s=s.replace('int main(){try{','''unsigned submissions=0;
uintptr_t __cdecl Context(unsigned){return 1;}
uintptr_t __thiscall Position(uintptr_t,float*){return 1;}
unsigned __thiscall Owner(uintptr_t){return 0;}
void __cdecl Submit(wht::DropBuffer131* b,unsigned mode){Check(b&&b->table==7&&!b->position&&!b->reserved&&b->size==6&&b->capacity==6&&!mode,"native datastore ABI");const auto* bytes=static_cast<const unsigned char*>(b->data);Check(bytes[0]==0x50&&bytes[1]==1,"borrowed payload");++submissions;}
int main(){try{''');# __thiscall standalone mock disallowed? existing native uses thiscall pointer; nonmember definition __thiscall MSVC supports only member? use nullptr cast? actual definitions compiler supports? avoid unused position owner via typed cast context never called.
s=s.replace('uintptr_t __thiscall Position(uintptr_t,float*){return 1;}\nunsigned __thiscall Owner(uintptr_t){return 0;}\n','')
s=s.replace(' int slot=99;', ''' wht::DropNetworkApi131 native;native.context=Context;native.send=Submit;native.position=reinterpret_cast<decltype(native.position)>(Context);native.owner=reinterpret_cast<decltype(native.owner)>(Context);native.table=7;Check(wht::SubmitAllianceSpam131(native,1,0xff0000ffu)&&wht::SubmitAllianceSpam131(native,1,0)&&submissions==2,"sender ABI");Check(!wht::SubmitAllianceSpam131(native,16,0)&&!wht::SubmitAllianceSpam131(native,1,4)&&submissions==2,"invalid no submission");
 int slot=99;''')
p.write_text(s,encoding='utf-8-sig')
