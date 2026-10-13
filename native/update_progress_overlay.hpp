#pragma once
#include <windows.h>
#include <string>
#include <algorithm>
namespace wht {
// A separate, click-through menu caption. The native version frame is untouched.
struct UpdateProgressOverlay {
    HWND window=nullptr;std::wstring caption;COLORREF color=RGB(0,255,0);
    static RECT Placement(int width,int height){
        // The yellow region in the reference spans 28%..56% of the client.
        const int line=(std::max)(18,height/36);
        return {width*28/100,height-line,width*56/100,height};
    }
    void Paint(HDC dc,RECT area){
        FillRect(dc,&area,static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
        auto font=CreateFontW(-(std::max)(12,static_cast<int>(area.bottom)*3/4),0,0,0,FW_NORMAL,FALSE,FALSE,FALSE,
            DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,NONANTIALIASED_QUALITY,DEFAULT_PITCH,L"Arial");
        auto old=SelectObject(dc,font);SetBkMode(dc,TRANSPARENT);SetTextColor(dc,color);
        DrawTextW(dc,caption.c_str(),-1,&area,DT_CENTER|DT_VCENTER|DT_SINGLELINE|DT_END_ELLIPSIS|DT_NOPREFIX);
        SelectObject(dc,old);DeleteObject(font);
    }
    static LRESULT CALLBACK Procedure(HWND w,UINT message,WPARAM wp,LPARAM lp){
        if(message==WM_NCCREATE)SetWindowLongPtrW(w,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams));
        auto self=reinterpret_cast<UpdateProgressOverlay*>(GetWindowLongPtrW(w,GWLP_USERDATA));
        if(message==WM_NCHITTEST)return HTTRANSPARENT;
        if(message==WM_MOUSEACTIVATE)return MA_NOACTIVATE;
        if(message==WM_ERASEBKGND)return 1;
        if(self&&message==WM_PAINT){PAINTSTRUCT ps{};auto dc=BeginPaint(w,&ps);RECT r{};GetClientRect(w,&r);self->Paint(dc,r);EndPaint(w,&ps);return 0;}
        return DefWindowProcW(w,message,wp,lp);
    }
    void Hide(){if(window)ShowWindow(window,SW_HIDE);}
    void Reset(){if(window)DestroyWindow(window);window=nullptr;caption.clear();}
    void Present(HMODULE instance,HWND owner,const std::string& colored,bool visible){
        if(!visible||colored.empty()||IsIconic(owner)||GetForegroundWindow()!=owner){Hide();return;}
        RECT client{};POINT origin{};
        if(!GetClientRect(owner,&client)||!ClientToScreen(owner,&origin)){Hide();return;}
        auto plain=colored;
        color=plain.find("|cffff5c5c")==0?RGB(255,92,92):RGB(0,255,0);
        if(plain.size()>=10&&plain.compare(0,2,"|c")==0)plain.erase(0,10);
        if(plain.size()>=2&&plain.compare(plain.size()-2,2,"|r")==0)plain.resize(plain.size()-2);
        const auto end=plain.find(" - ",plain.find("100%"));if(end!=std::string::npos)plain.resize(end);
        const int size=MultiByteToWideChar(CP_UTF8,0,plain.c_str(),-1,nullptr,0);
        if(size<=0){Hide();return;}
        std::wstring desired(size,L'\0');MultiByteToWideChar(CP_UTF8,0,plain.c_str(),-1,desired.data(),size);desired.resize(size-1);
        if(!window){
            WNDCLASSW wc{};wc.lpfnWndProc=Procedure;wc.hInstance=instance;wc.lpszClassName=L"WHTStartupProgress";
            if(!RegisterClassW(&wc)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)return;
            window=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE|WS_EX_TRANSPARENT|WS_EX_LAYERED,
                wc.lpszClassName,L"",WS_POPUP,0,0,1,1,owner,nullptr,instance,this);
            if(!window)return;
            if(!SetLayeredWindowAttributes(window,RGB(0,0,0),255,LWA_COLORKEY)){Reset();return;}
        }
        bool changed=caption!=desired;caption=std::move(desired);
        auto pos=Placement(client.right,client.bottom);
        RECT before{};GetWindowRect(window,&before);
        bool moved=before.left!=origin.x+pos.left||before.top!=origin.y+pos.top||before.right-before.left!=pos.right-pos.left||before.bottom-before.top!=pos.bottom-pos.top;
        SetWindowPos(window,HWND_TOP,origin.x+pos.left,origin.y+pos.top,pos.right-pos.left,pos.bottom-pos.top,SWP_NOACTIVATE|SWP_SHOWWINDOW);
        if(changed||moved)InvalidateRect(window,nullptr,FALSE);
    }
};
}
