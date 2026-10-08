#pragma once
#include <string>
#include <string_view>
#include <utility>
namespace wht {
// Native HP/MP labels are "%u / %u". Do not touch arbitrary labels, color
// escapes, negative/decimal values, or already formatted strings.
inline bool GroupLifeMana(std::string_view text,std::string& result){
    if(text.empty()||text.size()>128)return false;
    size_t begin[2]{},end[2]{},p=0;
    auto spaces=[&](){while(p<text.size()&&text[p]==' ')++p;};
    spaces();
    for(int n=0;n<2;++n){
        begin[n]=p;while(p<text.size()&&text[p]>='0'&&text[p]<='9')++p;end[n]=p;
        if(p==begin[n]||p-begin[n]>20)return false;
        spaces();if(n==0){if(p==text.size()||text[p++]!='/')return false;spaces();}
    }
    if(p!=text.size())return false;
    std::string formatted;formatted.reserve(text.size()+12);
    size_t copied=0;
    for(int n=0;n<2;++n){
        formatted.append(text.substr(copied,begin[n]-copied));
        for(size_t i=begin[n];i<end[n];++i){
            if(i>begin[n]&&(end[n]-i)%3==0)formatted.push_back(',');
            formatted.push_back(text[i]);
        }
        copied=end[n];
    }
    formatted.append(text.substr(copied));result=std::move(formatted);return true;
}
}
