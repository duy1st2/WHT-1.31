#pragma once
#include <windows.h>
#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace wht {
struct ResourceRow {
    unsigned slot=0;std::wstring name;unsigned color=12;
    int gold=0,lumber=0,food=0,cap=0;bool left=false;
};
struct ResourceTable {bool ready=false;std::vector<ResourceRow> rows;};
inline std::wstring ResourceNumber(int value){
    auto result=std::to_wstring(value);const size_t start=value<0?1:0;
    for(size_t pos=result.size();pos>start+3;){pos-=3;result.insert(pos,1,L',');}
    return result;
}
inline std::wstring ResourceName(const std::wstring& text){
    std::wstring result;
    for(size_t i=0;i<text.size()&&result.size()<64;){
        if(text[i]==L'|'&&i+1<text.size()){
            if((text[i+1]==L'c'||text[i+1]==L'C')&&i+10<=text.size()){
                bool hex=true;for(size_t j=i+2;j<i+10;++j)if(!((text[j]>=L'0'&&text[j]<=L'9')||(text[j]>=L'a'&&text[j]<=L'f')||(text[j]>=L'A'&&text[j]<=L'F')))hex=false;
                if(hex){i+=10;continue;}
            }
            if(text[i+1]==L'r'||text[i+1]==L'R'){i+=2;continue;}
        }
        const auto c=text[i++];result+=c<32||c==127?L' ':c;
    }
    return result;
}
// All methods run on the existing game window thread. No game frame is owned
// or modified here. The popup is layered/click-through and never takes focus.
struct ResourceOverlay {
    HWND window=nullptr;uintptr_t root=0;bool expanded=true;ULONGLONG nextRead=0;
    ResourceTable table;
    static constexpr int Width=600,Line=24;
    int Height()const{return expanded?76+Line*static_cast<int>(table.rows.size()):28;}
    void Paint(HDC dc,RECT client)const{
        HBRUSH background=CreateSolidBrush(RGB(24,28,34));FillRect(dc,&client,background);DeleteObject(background);
        const auto old=SelectObject(dc,GetStockObject(DEFAULT_GUI_FONT));SetBkMode(dc,TRANSPARENT);
        auto label=[&](const std::wstring& text,int left,int right,int y,COLORREF color,UINT align=DT_LEFT){
            SetTextColor(dc,color);RECT r{left,y,right,y+Line};DrawTextW(dc,text.c_str(),static_cast<int>(text.size()),&r,align|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS|DT_NOPREFIX);
        };
        label(expanded?L"Show Resource  [Tab: collapse]":L"Show Resource  [Tab: expand]",10,Width-10,2,RGB(235,238,243));
        if(expanded){
            label(L"Slot / Player",10,250,28,RGB(175,185,199));label(L"Gold",255,350,28,RGB(255,221,72),DT_RIGHT);
            label(L"Lumber",355,455,28,RGB(94,214,94),DT_RIGHT);label(L"Food",460,Width-10,28,RGB(247,165,139),DT_RIGHT);
            int y=52;
            for(const auto& row:table.rows){
                const auto gray=RGB(135,135,135);
                label(std::to_wstring(row.slot+1)+L"  "+row.name+(row.left?L" (left)":L""),10,250,y,row.left?gray:RGB(238,238,238));
                label(ResourceNumber(row.gold),255,350,y,row.left?gray:RGB(255,221,72),DT_RIGHT);
                label(ResourceNumber(row.lumber),355,455,y,row.left?gray:RGB(94,214,94),DT_RIGHT);
                const auto food=ResourceNumber(row.food)+(row.cap>0?L" / "+ResourceNumber(row.cap):L"");
                label(food,460,Width-10,y,row.left?gray:RGB(247,165,139),DT_RIGHT);y+=Line;
            }
            if(table.rows.empty())label(L"No eligible players",10,Width-10,52,RGB(175,185,199));
        }
        SelectObject(dc,old);
    }
    static LRESULT CALLBACK Procedure(HWND w,UINT message,WPARAM wp,LPARAM lp){
        if(message==WM_NCCREATE)SetWindowLongPtrW(w,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams));
        auto self=reinterpret_cast<ResourceOverlay*>(GetWindowLongPtrW(w,GWLP_USERDATA));
        if(message==WM_NCHITTEST)return HTTRANSPARENT;
        if(message==WM_MOUSEACTIVATE)return MA_NOACTIVATE;
        if(message==WM_ERASEBKGND)return 1;
        if(self&&message==WM_PRINTCLIENT){RECT r{};GetClientRect(w,&r);self->Paint(reinterpret_cast<HDC>(wp),r);return 0;}
        if(self&&message==WM_PAINT){
            PAINTSTRUCT ps{};HDC dc=BeginPaint(w,&ps);RECT r{};GetClientRect(w,&r);
            HDC buffer=CreateCompatibleDC(dc);HBITMAP bitmap=CreateCompatibleBitmap(dc,r.right,r.bottom);
            if(buffer&&bitmap){const auto old=SelectObject(buffer,bitmap);self->Paint(buffer,r);BitBlt(dc,0,0,r.right,r.bottom,buffer,0,0,SRCCOPY);SelectObject(buffer,old);}
            else self->Paint(dc,r);
            if(bitmap)DeleteObject(bitmap);if(buffer)DeleteDC(buffer);EndPaint(w,&ps);return 0;
        }
        return DefWindowProcW(w,message,wp,lp);
    }
    bool Create(HMODULE instance,HWND owner){
        if(window)return true;
        WNDCLASSW wc{};wc.lpfnWndProc=Procedure;wc.hInstance=instance;wc.lpszClassName=L"WHTResourceOverlay";
        if(!RegisterClassW(&wc)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)return false;
        window=CreateWindowExW(WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE|WS_EX_TRANSPARENT|WS_EX_LAYERED,wc.lpszClassName,L"ETS Show Resource",WS_POPUP,0,0,Width,Height(),owner,nullptr,instance,this);
        if(!window)return false;
        if(!SetLayeredWindowAttributes(window,0,240,LWA_ALPHA)){DestroyWindow(window);window=nullptr;return false;}
        return true;
    }
    void Hide(){if(window)ShowWindow(window,SW_HIDE);table={};nextRead=0;}
    void Reset(){if(window)DestroyWindow(window);window=nullptr;root=0;expanded=true;nextRead=0;table={};}
    void Bind(uintptr_t current){if(root!=current){Reset();root=current;}}
    bool Toggle(unsigned vk,bool enabled,bool typing,bool foreground,bool modifiers){
        if(vk!=VK_TAB||!root||!enabled||typing||!foreground||modifiers)return false;
        expanded=!expanded;nextRead=0;return true;
    }
    bool Present(HMODULE instance,HWND owner){
        RECT client{};POINT origin{};
        if(!GetClientRect(owner,&client)||client.right<Width+16||client.bottom<Height()+70||!ClientToScreen(owner,&origin)){Hide();return false;}
        if(!Create(instance,owner))return false;
        SetWindowPos(window,HWND_TOP,origin.x+8,origin.y+60,Width,Height(),SWP_NOACTIVATE|SWP_SHOWWINDOW);
        InvalidateRect(window,nullptr,FALSE);return true;
    }
};
}
