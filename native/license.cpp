#include "common.hpp"
#include <winhttp.h>
#include <memory>

namespace wht {
struct CloseHttp {void operator()(void* h)const{if(h)WinHttpCloseHandle(h);}};
using Http=std::unique_ptr<void,CloseHttp>;
static std::string Encode(const std::string& s){
    std::string out;const char* h="0123456789ABCDEF";
    for(unsigned char c:s){if((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='-')out+=c;else{out+='%';out+=h[c>>4];out+=h[c&15];}}return out;
}
bool Activate(const Config& c,Lease& lease,std::wstring& error) {
    lease={};error=L"License server unavailable";
    if(c.key.empty()){error=L"Configure [License] Key in WHT.ini";return false;}
    URL_COMPONENTS u{};u.dwStructSize=sizeof(u);u.dwHostNameLength=u.dwUrlPathLength=u.dwExtraInfoLength=u.dwUserNameLength=u.dwPasswordLength=DWORD(-1);
    if(!WinHttpCrackUrl(c.server.c_str(),0,0,&u)){error=L"Invalid license server URL";return false;}
    std::wstring host(u.lpszHostName,u.dwHostNameLength),path(u.lpszUrlPath?u.lpszUrlPath:L"",u.dwUrlPathLength);
    bool local=host==L"127.0.0.1"||host==L"localhost"||host==L"[::1]"||host==L"::1";
    if(u.dwUserNameLength||u.dwPasswordLength||u.dwExtraInfoLength||
       (u.nScheme!=INTERNET_SCHEME_HTTPS && !(local&&u.nScheme==INTERNET_SCHEME_HTTP))){error=L"License server requires HTTPS (HTTP allowed only for localhost)";return false;}
    while(!path.empty()&&path.back()==L'/')path.pop_back();path+=L"/v1/activate";
    wchar_t guid[256]{};DWORD size=sizeof(guid);
    if(RegGetValueW(HKEY_LOCAL_MACHINE,L"SOFTWARE\\Microsoft\\Cryptography",L"MachineGuid",RRF_RT_REG_SZ|RRF_SUBKEY_WOW6464KEY,nullptr,guid,&size)!=ERROR_SUCCESS){error=L"Cannot read machine identity";return false;}
    auto identity=Utf8(std::wstring(L"WHT:v1:")+guid);auto machine=Sha256(identity.data(),identity.size());
    if(machine.empty())return false;
    Http session(WinHttpOpen(L"WHT/0.1",WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,WINHTTP_NO_PROXY_NAME,WINHTTP_NO_PROXY_BYPASS,0));if(!session)return false;
    WinHttpSetTimeouts(session.get(),3000,3000,3000,3000);
    Http conn(WinHttpConnect(session.get(),host.c_str(),u.nPort,0));if(!conn)return false;
    Http req(WinHttpOpenRequest(conn.get(),L"POST",path.c_str(),nullptr,WINHTTP_NO_REFERER,WINHTTP_DEFAULT_ACCEPT_TYPES,u.nScheme==INTERNET_SCHEME_HTTPS?WINHTTP_FLAG_SECURE:0));if(!req)return false;
    DWORD redirect=WINHTTP_OPTION_REDIRECT_POLICY_NEVER;WinHttpSetOption(req.get(),WINHTTP_OPTION_REDIRECT_POLICY,&redirect,sizeof(redirect));
    const std::string body="key="+Encode(Utf8(c.key))+"&machine="+machine;
    if(!WinHttpSendRequest(req.get(),L"Content-Type: application/x-www-form-urlencoded\r\n",DWORD(-1),(void*)body.data(),static_cast<DWORD>(body.size()),static_cast<DWORD>(body.size()),0)||!WinHttpReceiveResponse(req.get(),nullptr))return false;
    DWORD status=0;size=sizeof(status);
    if(!WinHttpQueryHeaders(req.get(),WINHTTP_QUERY_STATUS_CODE|WINHTTP_QUERY_FLAG_NUMBER,nullptr,&status,&size,nullptr))return false;
    if(status!=200){error=L"License denied / HTTP "+std::to_wstring(status);return false;}
    std::string response;char buffer[1024];DWORD got=0;
    for(;;){if(!WinHttpReadData(req.get(),buffer,sizeof(buffer),&got))return false;if(!got)break;response.append(buffer,got);if(response.size()>4096)return false;}
    if(!ParseLease(response,lease)){error=L"Invalid response from license server";return false;}
    error.clear();return true;
}
}
