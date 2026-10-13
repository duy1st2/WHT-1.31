#pragma once
#include "common.hpp"
#include <sstream>
namespace wht {
struct MthDrawCommand129 {const wchar_t* name;const char* title;const wchar_t* keys[5];unsigned count;};
inline const MthDrawCommand129* FindMthDrawCommand129(std::wstring name){
 FoldCommandName(name);
 static const MthDrawCommand129 commands[]={
 {L"#shielddraw","Shield button",{L"ShieldDraw"},1},{L"#icedraw","Ice button",{L"IceDraw"},1},
 {L"#wilddraw","Wild button",{L"WildDraw"},1},{L"#frozendraw","Frozen button",{L"FrozenDraw"},1},
 {L"#phoenixdraw","Phoenix button",{L"PhoenixDraw"},1},{L"#gokudraw","Goku button",{L"PickGokuDraw"},1},
 {L"#ga1draw","Chicken 1 button",{L"AutoGaDraw1"},1},{L"#ga2draw","Chicken 2 button",{L"AutoGaDraw2"},1},{L"#ga3draw","Chicken 3 button",{L"AutoGaDraw3"},1},
 {L"#soi1draw","Wolf 1 button",{L"AutoSoiDraw1"},1},{L"#soi2draw","Wolf 2 button",{L"AutoSoiDraw2"},1},{L"#soi3draw","Wolf 3 button",{L"AutoSoiDraw3"},1},
 {L"#mthdraw","MTH crafting buttons",{L"ShieldDraw",L"IceDraw",L"WildDraw",L"FrozenDraw",L"PhoenixDraw"},5},
 {L"#gadraw","Chicken buttons",{L"AutoGaDraw1",L"AutoGaDraw2",L"AutoGaDraw3"},3},
 {L"#soidraw","Wolf buttons",{L"AutoSoiDraw1",L"AutoSoiDraw2",L"AutoSoiDraw3"},3}};
 for(const auto& c:commands)if(name==c.name)return &c;return nullptr;
}
inline bool ParseMthDrawValue129(std::wistream& in,bool anyEnabled,std::wstring& value){
 std::wstring token,extra;if(!(in>>token)){value=anyEnabled?L"0":L"1";return true;}
 if(in>>extra)return false;FoldCommandName(token);
 if(token==L"1"||token==L"on"||token==L"true")value=L"1";
 else if(token==L"0"||token==L"off"||token==L"false")value=L"0";else return false;return true;
}
}
