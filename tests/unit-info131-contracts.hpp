#pragma once
#include "unit_info131.hpp"
inline void CheckUnitInfo131(){
 using namespace wht;UnitInfo131 out{99,1,2};unsigned calls=0;InventoryInfoSource131 source{123,{1,2}};
 auto selected=[&](){++calls;return source;};
 auto read=[](uintptr_t unit,UnitInfo131& value){check(unit==123,"unit-info reader uses selected unit");value={0x48303031,12.5f,-20.25f};return true;};
 check(CaptureUnitInfo131(true,selected,read,out)&&calls==2&&out.code==0x48303031&&out.x==12.5f,"unit-info stable capture publishes code and coordinates");
 check(UnitInfoText131(out,false)=="Unit ID: [H001] (0x48303031)"&&UnitInfoText131(out,true)=="X: 12.50000, Y: -20.25000","unit info formatting matches 1.29 exactly");
 const auto previous=out;check(!CaptureUnitInfo131(true,selected,[&](uintptr_t,UnitInfo131& value){value={1,3,4};source.id={3,4};return true;},out)&&out.code==previous.code,"recycled identity cannot publish stale data");
 source={123,{1,2}};check(!CaptureUnitInfo131(true,selected,[&](uintptr_t,UnitInfo131& value){value={1,3,4};source.unit=456;return true;},out),"selection changes during read refused");
 source={123,{1,2}};check(!CaptureUnitInfo131(true,selected,[](uintptr_t,UnitInfo131& value){value={1,NAN,2};return true;},out),"nonfinite coordinates refused");
 check(!CaptureUnitInfo131(false,selected,[](uintptr_t,UnitInfo131& value){value={};return true;},out),"missing rawcode refused");
 source={};check(!CaptureUnitInfo131(false,selected,read,out),"empty selection refused before native read");
 check(UnitInfoArgs131(L"")&&UnitInfoArgs131(L" \t"),"unit-info commands accept no arguments");
 for(auto args:{L"1",L"x",L"\n",L"\r"})check(!UnitInfoArgs131(args),"unit-info commands reject arguments and line breaks");
 check(!UnitInfoArgs131(std::wstring(L"\0",1)),"unit-info commands reject embedded NUL");
}
