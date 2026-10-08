#pragma once
#include <array>
#include <cstdint>
#include <sstream>
#include <string>
namespace wht {
struct ResourceTransfer {unsigned slot=0;uint32_t gold=0,lumber=0;};
inline bool TransferNumber(const std::wstring& text,uint32_t limit,uint32_t& result){
    if(text.empty()||text.size()>10)return false;
    uint32_t value=0;
    for(const auto ch:text){
        if(ch<L'0'||ch>L'9')return false;
        const auto digit=static_cast<uint32_t>(ch-L'0');
        if(digit>limit||value>(limit-digit)/10)return false;
        value=value*10+digit;
    }
    result=value;return true;
}
inline bool ParseResourceTransfer(const std::wstring& command,ResourceTransfer& result){
    std::wistringstream in(command);std::wstring name,slot,amount,extra;
    if(!(in>>name>>slot>>amount)||(in>>extra)||(name!=L"#gg"&&name!=L"#gl"))return false;
    uint32_t recipient=0,value=0;
    if(!TransferNumber(slot,16,recipient)||!recipient||!TransferNumber(amount,0x7fffffffu,value)||!value)return false;
    result={recipient-1,name==L"#gg"?value:0,name==L"#gl"?value:0};return true;
}
inline bool ValidResourceTransfer(const ResourceTransfer& request){
    return request.slot<16&&((request.gold>0&&request.gold<=0x7fffffffu&&request.lumber==0)||
        (request.lumber>0&&request.lumber<=0x7fffffffu&&request.gold==0));
}
inline std::array<uint8_t,10> ResourceTransferBytes(const ResourceTransfer& request){
    std::array<uint8_t,10> out{{0x51,static_cast<uint8_t>(request.slot)}};
    for(unsigned i=0;i<4;++i){out[2+i]=static_cast<uint8_t>(request.gold>>(8*i));out[6+i]=static_cast<uint8_t>(request.lumber>>(8*i));}
    return out;
}
}
