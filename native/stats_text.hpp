#pragma once
#include <string>
#include <string_view>
#include <utility>
namespace wht {
// Native stat labels: signed integer, optional damage range, optional colored
// signed bonus (armor may have one fractional digit). Preserve all markup.
inline bool GroupStatsText(std::string_view text,std::string& result){
    if(text.empty()||text.size()>128||text.front()=='+')return false;
    size_t p=0;std::string out;out.reserve(text.size()+20);
    auto number=[&](bool fraction){
        if(p<text.size()&&(text[p]=='-'||text[p]=='+'))out.push_back(text[p++]);
        const auto begin=p;while(p<text.size()&&text[p]>='0'&&text[p]<='9')++p;
        if(p==begin||p-begin>20)return false;
        for(size_t i=begin;i<p;++i){if(i>begin&&(p-i)%3==0)out.push_back(',');out.push_back(text[i]);}
        if(fraction&&p<text.size()&&text[p]=='.'){
            out.push_back(text[p++]);if(p==text.size()||text[p]<'0'||text[p]>'9')return false;out.push_back(text[p++]);
        }
        return true;
    };
    if(!number(false))return false;
    if(text.substr(p,3)==" - "){out+=" - ";p+=3;if(!number(false))return false;}
    if(p<text.size()){
        // These are the native positive/negative bonus colors, not arbitrary
        // color escapes or texture/control sequences.
        if(text.substr(p,12)==" |CFF00FF00+"){out+=" |CFF00FF00+";p+=12;if(p==text.size()||text[p]<'0'||text[p]>'9')return false;}
        else if(text.substr(p,11)==" |CFFFF0000"){out+=" |CFFFF0000";p+=11;if(p==text.size()||text[p]!='-')return false;}
        else return false;
        if(!number(true)||text.substr(p,2)!="|R")return false;
        out+="|R";p+=2;
    }
    if(p!=text.size()||out.size()>128)return false;
    result=std::move(out);return true;
}
}
