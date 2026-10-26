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
    // Keep command syntax out of player-facing notifications.
    std::string normalized;
    for(size_t i=0;i<plain.size();){
        if(plain[i]=='#'&&i+1<plain.size()&&word(plain[i+1])){
            ++i;while(i<plain.size()&&word(plain[i]))++i;
            continue;
        }
        size_t end=i;while(end<plain.size()&&word(plain[end]))++end;
        if(end==i){normalized+=plain[i++];continue;}
        auto token=plain.substr(i,end-i),upper=token;
        for(auto& c:upper)if(c>='a'&&c<='z')c-=32;
        normalized+=(upper=="ENABLE"||upper=="ENABLED")?"ON":
            (upper=="DISABLE"||upper=="DISABLED")?"OFF":token;
        i=end;
    }
    // Remove empty command annotations, then tidy their surrounding spaces.
    for(size_t i=0;i<normalized.size();){
        if(normalized[i]=='('){
            size_t end=i+1;while(end<normalized.size()&&(normalized[end]==' '||normalized[end]=='\t'))++end;
            if(end<normalized.size()&&normalized[end]==')'){normalized.erase(i,end-i+1);continue;}
        }
        ++i;
    }
    plain.clear();
    for(char c:normalized){
        if(c==' '||c=='\t'){
            if(!plain.empty()&&plain.back()!=' '&&plain.back()!='\n')plain+=' ';
        }else plain+=c;
    }
    while(!plain.empty()&&plain.back()==' ')plain.pop_back();
    // Toggle notifications end in a status; separate it from the feature name.
    const auto statusSpace=plain.find_last_of(' ');
    if(statusSpace!=std::string::npos){
        auto status=plain.substr(statusSpace+1);
        for(auto& c:status)if(c>='a'&&c<='z')c-=32;
        if(status=="ON"||status=="OFF"){
            size_t nameEnd=statusSpace;
            while(nameEnd>0&&plain[nameEnd-1]==' ')--nameEnd;
            if(nameEnd>0&&word(plain[nameEnd-1]))plain.insert(nameEnd,":");
        }
    }
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
