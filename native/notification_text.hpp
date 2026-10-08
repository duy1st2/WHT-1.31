#pragma once
#include <string>
namespace wht {
inline std::wstring SettingFeedback(const std::wstring& name,std::wstring value){
    if(value.find(L'.')!=std::wstring::npos&&value.find_first_not_of(L"0123456789.-")==std::wstring::npos){
        while(!value.empty()&&value.back()==L'0')value.pop_back();
        if(!value.empty()&&value.back()==L'.')value.pop_back();
    }
    return L"Set "+name+L": "+value;
}
inline std::string NotificationText(const std::string& text){
    std::string plain;
    const auto hex=[](unsigned char c){return (c>='0'&&c<='9')||(c>='a'&&c<='f')||(c>='A'&&c<='F');};
    for(size_t i=0;i<text.size();){
        if(text[i]=='|'&&i+1<text.size()){
            if(text[i+1]=='r'||text[i+1]=='R'){i+=2;continue;}
            if((text[i+1]=='c'||text[i+1]=='C')&&i+10<=text.size()){
                bool valid=true;for(size_t j=i+2;j<i+10;++j)valid=valid&&hex(text[j]);
                if(valid){i+=10;continue;}
            }
        }
        plain+=text[i++];
    }
    // Callers may prepend [WHT] to a message that already carries the tag.
    // Normalize only repeated leading tags, after removing Warcraft colors.
    if(plain.compare(0,5,"[WHT]")==0){
        size_t next=5;
        while(next<plain.size()&&(plain[next]==' '||plain[next]=='\t'))++next;
        if(plain.compare(next,5,"[WHT]")==0){
            do {
                next+=5;
                while(next<plain.size()&&(plain[next]==' '||plain[next]=='\t'))++next;
            } while(plain.compare(next,5,"[WHT]")==0);
            plain="[WHT]"+(next<plain.size()?" "+plain.substr(next):std::string{});
        }
    }
    const auto word=[](unsigned char c){return c>=128||(c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_';};
    size_t valueAt=std::string::npos;
    const size_t prefix=plain.compare(0,6,"[WHT] ")==0?6:0;
    if(plain.compare(prefix,4,"Set ")==0){
        const auto colon=plain.find(':',prefix+4);
        if(colon!=std::string::npos){valueAt=colon+1;while(valueAt<plain.size()&&plain[valueAt]==' ')++valueAt;}
    }
    std::string out="|cffffff00";
    for(size_t i=0;i<plain.size();){
        if(i==valueAt)out+="|cff00ff00";
        size_t end=i;while(end<plain.size()&&word(plain[end]))++end;
        if(end==i){out+=plain[i++];continue;}
        auto token=plain.substr(i,end-i);auto upper=token;
        for(auto& c:upper)if(c>='a'&&c<='z')c-=32;
        if(upper=="ON"||upper=="OFF")out+=(upper=="ON"?"|cff00ff00":"|cffff0000")+token+(i>=valueAt?"|cff00ff00":"|cffffff00");
        else out+=token;
        i=end;
    }
    return out+"|r";
}
}
