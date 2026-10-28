#include "external_hud.hpp"
#include "hud_layout129.hpp"
#include <d3d11.h>
#include <dxgi1_2.h>
#include <d2d1_1.h>
#include <dwrite.h>
#include <dcomp.h>
#include <wrl/client.h>
#include <psapi.h>
#include <shellapi.h>
#include <cstdio>
#include <cmath>
#include <algorithm>
using Microsoft::WRL::ComPtr;
namespace {
LRESULT CALLBACK HudWindow(HWND window,UINT message,WPARAM wp,LPARAM lp){
    if(message==WM_NCHITTEST)return HTTRANSPARENT;
    if(message==WM_MOUSEACTIVATE)return MA_NOACTIVATE;
    return DefWindowProcW(window,message,wp,lp);
}
HWND CreateHudWindow(HINSTANCE instance){
    WNDCLASSW wc{};wc.hInstance=instance;wc.lpfnWndProc=HudWindow;wc.lpszClassName=L"WHT.ExternalHud.DX11";
    if(!RegisterClassW(&wc)&&GetLastError()!=ERROR_CLASS_ALREADY_EXISTS)return nullptr;
    HWND window=CreateWindowExW(WS_EX_LAYERED|WS_EX_TOPMOST|WS_EX_TRANSPARENT|WS_EX_TOOLWINDOW|WS_EX_NOACTIVATE|WS_EX_NOREDIRECTIONBITMAP,
        wc.lpszClassName,L"WHT External HUD",WS_POPUP,0,0,800,600,nullptr,nullptr,instance,nullptr);
    if(window&&!SetLayeredWindowAttributes(window,0,255,LWA_ALPHA)){DestroyWindow(window);return nullptr;}
    return window;
}
class HudRenderer {
    ComPtr<ID3D11Device> device;ComPtr<IDXGISwapChain1> swap;
    ComPtr<ID2D1Factory1> factory;ComPtr<ID2D1Device> drawingDevice;ComPtr<ID2D1DeviceContext> context;
    ComPtr<IDCompositionDevice> composition;ComPtr<IDCompositionTarget> target;ComPtr<IDCompositionVisual> visual;
    ComPtr<ID2D1SolidColorBrush> foreground,shadow;ComPtr<IDWriteFactory> textFactory;
    ComPtr<IDWriteTextFormat> timerFont,memoryFont;UINT width=0,height=0;
    bool Surface(){
        ComPtr<IDXGISurface> surface;if(FAILED(swap->GetBuffer(0,IID_PPV_ARGS(&surface))))return false;
        auto properties=D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET|D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
            D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_PREMULTIPLIED),96,96);
        ComPtr<ID2D1Bitmap1> bitmap;if(FAILED(context->CreateBitmapFromDxgiSurface(surface.Get(),&properties,&bitmap)))return false;
        context->SetTarget(bitmap.Get());return true;
    }
public:
    bool Init(HWND window){
        D3D_FEATURE_LEVEL level{};
        if(FAILED(D3D11CreateDevice(nullptr,D3D_DRIVER_TYPE_HARDWARE,nullptr,D3D11_CREATE_DEVICE_BGRA_SUPPORT,nullptr,0,D3D11_SDK_VERSION,&device,&level,nullptr)))return false;
        ComPtr<IDXGIDevice> dxgi;if(FAILED(device.As(&dxgi)))return false;
        ComPtr<IDXGIAdapter> adapter;ComPtr<IDXGIFactory2> dxgiFactory;
        if(FAILED(dxgi->GetAdapter(&adapter))||FAILED(adapter->GetParent(IID_PPV_ARGS(&dxgiFactory))))return false;
        DXGI_SWAP_CHAIN_DESC1 desc{};desc.Width=800;desc.Height=600;desc.Format=DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.SampleDesc.Count=1;desc.BufferUsage=DXGI_USAGE_RENDER_TARGET_OUTPUT;desc.BufferCount=2;
        desc.SwapEffect=DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;desc.AlphaMode=DXGI_ALPHA_MODE_PREMULTIPLIED;
        if(FAILED(dxgiFactory->CreateSwapChainForComposition(device.Get(),&desc,nullptr,&swap)))return false;
        D2D1_FACTORY_OPTIONS options{};
        if(FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,__uuidof(ID2D1Factory1),&options,reinterpret_cast<void**>(factory.GetAddressOf())))||
            FAILED(factory->CreateDevice(dxgi.Get(),&drawingDevice))||FAILED(drawingDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE,&context)))return false;
        if(FAILED(DCompositionCreateDevice(dxgi.Get(),__uuidof(IDCompositionDevice),reinterpret_cast<void**>(composition.GetAddressOf())))||
            FAILED(composition->CreateTargetForHwnd(window,TRUE,&target))||FAILED(composition->CreateVisual(&visual))||
            FAILED(visual->SetContent(swap.Get()))||FAILED(target->SetRoot(visual.Get()))||FAILED(composition->Commit()))return false;
        if(FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED,__uuidof(IDWriteFactory),reinterpret_cast<IUnknown**>(textFactory.GetAddressOf()))))return false;
        if(FAILED(context->CreateSolidColorBrush(D2D1::ColorF(0xdc/255.f,0xda/255.f,0x3c/255.f,1),&foreground))||
            FAILED(context->CreateSolidColorBrush(D2D1::ColorF(0,0,0,1),&shadow)))return false;
        width=800;height=600;return Surface()&&Resize(800,600);
    }
    bool Resize(UINT w,UINT h){
        if(!w||!h||w>16384||h>16384)return false;
        if(width!=w||height!=h){
            context->SetTarget(nullptr);
            if(FAILED(swap->ResizeBuffers(2,w,h,DXGI_FORMAT_B8G8R8A8_UNORM,0))||!Surface())return false;
            width=w;height=h;
        }
        timerFont.Reset();memoryFont.Reset();
        const float timerSize=std::max(10.f,float(h)*.016f/.6f),memorySize=std::max(9.f,float(h)*.0115f/.6f);
        if(FAILED(textFactory->CreateTextFormat(L"Segoe UI",nullptr,DWRITE_FONT_WEIGHT_NORMAL,DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,timerSize,L"en-us",&timerFont))||
            FAILED(textFactory->CreateTextFormat(L"Segoe UI",nullptr,DWRITE_FONT_WEIGHT_NORMAL,DWRITE_FONT_STYLE_NORMAL,DWRITE_FONT_STRETCH_NORMAL,memorySize,L"en-us",&memoryFont)))return false;
        timerFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);memoryFont->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
        timerFont->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);memoryFont->SetWordWrapping(DWRITE_WORD_WRAPPING_NO_WRAP);return true;
    }
    bool InspectPixels(){
        ComPtr<ID3D11Texture2D> source,staging;ComPtr<ID3D11DeviceContext> gpu;
        if(FAILED(swap->GetBuffer(0,IID_PPV_ARGS(&source))))return false;
        D3D11_TEXTURE2D_DESC desc{};source->GetDesc(&desc);desc.Usage=D3D11_USAGE_STAGING;desc.BindFlags=0;desc.MiscFlags=0;desc.CPUAccessFlags=D3D11_CPU_ACCESS_READ;
        if(FAILED(device->CreateTexture2D(&desc,nullptr,&staging)))return false;
        device->GetImmediateContext(&gpu);gpu->CopyResource(staging.Get(),source.Get());D3D11_MAPPED_SUBRESOURCE map{};
        if(FAILED(gpu->Map(staging.Get(),0,D3D11_MAP_READ,0,&map)))return false;
        unsigned visible=0;auto bytes=static_cast<const unsigned char*>(map.pData);
        const bool transparentCorner=bytes[3]==0;
        for(UINT y=0;y<height;++y)for(UINT x=0;x<width;++x)if(bytes[y*map.RowPitch+x*4+3])++visible;
        gpu->Unmap(staging.Get(),0);return transparentCorner&&visible>50&&visible<width*height/4;
    }
    bool Draw(const wht::HudPacket& packet,const std::wstring& memory,bool inspect=false){
        context->BeginDraw();context->Clear(D2D1::ColorF(0,0,0,0));
        const float cx=float(width)*wht::HudCenterX129((packet.flags&wht::HudHideTime)!=0)/.8f;
        auto text=[&](const wchar_t* value,IDWriteTextFormat* font,float y,float center){
            auto bounds=D2D1::RectF(center-float(width)*.15f,y,center+float(width)*.15f,y+float(height)*.04f);
            auto offset=bounds;offset.left+=1;offset.right+=1;offset.top+=1;offset.bottom+=1;
            context->DrawText(value,UINT32(wcslen(value)),font,&offset,shadow.Get());
            context->DrawText(value,UINT32(wcslen(value)),font,&bounds,foreground.Get());
        };
        if(packet.flags&wht::HudTimer){wchar_t timer[64]{};swprintf_s(timer,L"%02u:%02u:%02u",packet.seconds/3600,(packet.seconds/60)%60,packet.seconds%60);
            text(timer,timerFont.Get(),float(height)*(.6f-wht::TimerCenterY129)/.6f-float(height)*.012f,cx);}
        if(packet.flags&wht::HudMemory)text(memory.c_str(),memoryFont.Get(),float(height)*(.6f-wht::MemoryCenterY129)/.6f-float(height)*.008f,cx);
        if(FAILED(context->EndDraw())||(inspect&&!InspectPixels())||FAILED(swap->Present(0,0)))return false;
        return SUCCEEDED(composition->Commit());
    }
};
int Run(HINSTANCE instance,const std::wstring& name,bool selfTest){
    HANDLE mapping=nullptr,process=nullptr;wht::HudShared* shared=nullptr;
    if(!selfTest){mapping=OpenFileMappingW(FILE_MAP_ALL_ACCESS,FALSE,name.c_str());if(!mapping)return 2;
        shared=static_cast<wht::HudShared*>(MapViewOfFile(mapping,FILE_MAP_ALL_ACCESS,0,0,sizeof(wht::HudShared)));if(!shared){CloseHandle(mapping);return 3;}}
    HWND window=CreateHudWindow(instance);HudRenderer renderer;int result=0;
    if(!window||!renderer.Init(window))result=4;
    else if(selfTest){wht::HudPacket packet{};packet.flags=wht::HudMemory|wht::HudTimer;packet.seconds=3661;
        if(!renderer.Draw(packet,L"Memory(MB): 123.45 / 2048 (6.03%)",true)||!renderer.Resize(1280,720)||
            !renderer.Draw(packet,L"Memory(MB): 123.45 / 2048 (6.03%)",true))result=5;
        const auto styles=GetWindowLongPtrW(window,GWL_EXSTYLE);
        if(!(styles&WS_EX_LAYERED)||!(styles&WS_EX_TRANSPARENT)||!(styles&WS_EX_NOACTIVATE)||SendMessageW(window,WM_NCHITTEST,0,0)!=HTTRANSPARENT)result=10;}
    else {
        wht::HudPacket packet{};DWORD lastPid=shared->ownerPid,lastMemory=0,lastSeconds=MAXDWORD,lastFlags=MAXDWORD,lastEpoch=0;
        RECT previous{};DWORD inactiveAt=0;bool shown=false;std::wstring memory;wht::WorkerPlacement placement;
        process=lastPid?OpenProcess(SYNCHRONIZE|PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,lastPid):nullptr;
        if(!process)result=6;
        for(;process;){
            if(WaitForSingleObject(process,0)!=WAIT_TIMEOUT)break;
            MSG message{};while(PeekMessageW(&message,nullptr,0,0,PM_REMOVE)){TranslateMessage(&message);DispatchMessageW(&message);}
            if(shared->magic!=wht::HudMagic||shared->version!=wht::HudVersion){result=12;break;}
            if(!wht::ReadHudPacket(*shared,packet)){Sleep(30);continue;}
            if(process&&(packet.pid!=lastPid||WaitForSingleObject(process,0)!=WAIT_TIMEOUT))break;
            placement.Refresh((packet.flags&wht::HudWorkerPreference)!=0,packet.mainProcessor);
            HWND game=reinterpret_cast<HWND>(uintptr_t(packet.window));DWORD pid=0;GetWindowThreadProcessId(game,&pid);
            RECT rect{};const DWORD now=GetTickCount();
            if(!(packet.flags&(wht::HudMemory|wht::HudTimer))){if(!inactiveAt)inactiveAt=now;if(DWORD(now-inactiveAt)>3000)break;}else inactiveAt=0;
            bool visible=pid==lastPid&&game&&IsWindowVisible(game)&&!IsIconic(game)&&GetForegroundWindow()==game&&
                DWORD(now-packet.updated)<1000&&(packet.flags&(wht::HudMemory|wht::HudTimer))&&GetClientRect(game,&rect)&&rect.right>0&&rect.bottom>0;
            if(!visible){if(shown){ShowWindow(window,SW_HIDE);shown=false;}InterlockedExchange(&shared->ackFlags,0);Sleep(50);continue;}
            POINT origin{};if(!ClientToScreen(game,&origin))break;OffsetRect(&rect,origin.x,origin.y);
            const bool moved=!EqualRect(&rect,&previous);const bool resized=rect.right-rect.left!=previous.right-previous.left||rect.bottom-rect.top!=previous.bottom-previous.top;
            if(resized&&!renderer.Resize(UINT(rect.right-rect.left),UINT(rect.bottom-rect.top))){result=7;break;}
            if((moved||!shown)&&!SetWindowPos(window,HWND_TOPMOST,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,SWP_NOACTIVATE)){result=11;break;}
            bool memoryChanged=false;
            if((packet.flags&wht::HudMemory)&&DWORD(now-lastMemory)>=500){
                PROCESS_MEMORY_COUNTERS counters{};counters.cb=sizeof(counters);
                if(!GetProcessMemoryInfo(process,&counters,sizeof(counters))){result=8;break;}
                const double mb=double(counters.WorkingSetSize)/(1024.*1024.);wchar_t text[128]{};
                #ifdef _WIN64
                swprintf_s(text,L"Memory(MB): %.2f",mb);
#else
                swprintf_s(text,L"Memory(MB): %.2f / 2048 (%.2f%%)",mb,mb/2048.*100.);
#endif
                memoryChanged=memory!=text;memory=text;lastMemory=now;
            }
            if(!shown||resized||memoryChanged||packet.seconds!=lastSeconds||packet.flags!=lastFlags||packet.epoch!=lastEpoch){
                if(!renderer.Draw(packet,memory)){result=9;break;}
                lastSeconds=packet.seconds;lastFlags=packet.flags;lastEpoch=packet.epoch;
            }
            if(!shown){ShowWindow(window,SW_SHOWNOACTIVATE);shown=true;}
            previous=rect;
            InterlockedExchange(&shared->ackFlags,LONG(packet.flags&(wht::HudMemory|wht::HudTimer)));
            InterlockedExchange(&shared->ackEpoch,LONG(packet.epoch));InterlockedExchange(&shared->ackTick,LONG(now));
            Sleep(30);
        }
    }
    if(shared){InterlockedExchange(&shared->ackFlags,0);UnmapViewOfFile(shared);}if(mapping)CloseHandle(mapping);if(process)CloseHandle(process);
    if(window)DestroyWindow(window);return result;
}
}
int WINAPI wWinMain(HINSTANCE instance,HINSTANCE,PWSTR,int){
    // Physical pixels: prevent DPI virtualization of the game client rectangle.
    auto dpi=reinterpret_cast<BOOL(WINAPI*)(HANDLE)>(GetProcAddress(GetModuleHandleW(L"user32.dll"),"SetProcessDpiAwarenessContext"));
    if(dpi)dpi(reinterpret_cast<HANDLE>(-4));else SetProcessDPIAware();
    int count=0;auto args=CommandLineToArgvW(GetCommandLineW(),&count);std::wstring name;bool selfTest=false;
    if(args){if(count==2&&!wcscmp(args[1],L"--self-test"))selfTest=true;else if(count==3&&!wcscmp(args[1],L"--mapping"))name=args[2];LocalFree(args);}
    if(!selfTest&&(name.rfind(L"Local\\WHT131.ExternalHud.",0)!=0||name.size()>160))return 1;
    CoInitializeEx(nullptr,COINIT_APARTMENTTHREADED);int result=Run(instance,name,selfTest);CoUninitialize();return result;
}
