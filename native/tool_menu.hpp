#pragma once
#include "mth_button_overlay.hpp"
#include "hud_action_toolbar.hpp"
#include "time_boss.hpp"
#include <array>
#include <algorithm>
#include <cmath>
namespace wht {
struct ToolMenuRect {float right,bottom,width,height;
 bool Hit(POINT p,int w,int h)const{if(w<=0||h<=0||p.x<0||p.y<0||p.x>=w||p.y>=h)return false;
 float x=float(p.x)*.8f/w,y=float(h-p.y)*.6f/h;return x>=right-width&&x<=right&&y>=bottom&&y<=bottom+height;}};
struct MenuPixelRect{float x,y,width,height;};
enum class MenuControlKind{Check,Key,Text,Number,Choice,Clear,Label};
struct ToolMenuControl{MenuControlKind kind;int page;const char* label;const wchar_t *section,*key,*fallback;
 MenuPixelRect rect,labelRect;float minimum,maximum;const wchar_t* dependency;bool supported;};
#include "tool_menu_layout.hpp"
#include "tool_language.hpp"
#include "tool_menu_help.hpp"
inline std::wstring MenuRead(const std::wstring& ini,const wchar_t* section,const wchar_t* key,const wchar_t* fallback){
 wchar_t value[1024]{};GetPrivateProfileStringW(section,key,fallback,value,1024,ini.c_str());return value;}
inline std::wstring MenuRead(const Config& c,const wchar_t* section,const wchar_t* key,const wchar_t* fallback){
 if(!c.profile)return fallback;
 auto lower=[](std::wstring s){for(auto& ch:s)ch=towlower(ch);return s;};
 auto group=c.profile->find(lower(section));if(group==c.profile->end())return fallback;
 auto entry=group->second.find(lower(key));return entry==group->second.end()?fallback:entry->second;}
inline int MenuEngineKey(unsigned vk){for(int code=0;code<=0x30b;++code)if(vk&&EngineKeyToVirtualKey(code)==vk)return code;return -1;}
inline std::string MenuKeyName(const std::wstring& code){auto vk=EngineKeyToVirtualKey(int(wcstol(code.c_str(),nullptr,10)));if(!vk)return {};
 wchar_t name[64]{};auto scan=MapVirtualKeyW(vk,MAPVK_VK_TO_VSC);if(vk>=VK_PRIOR&&vk<=VK_DELETE)scan|=0x100;
 return GetKeyNameTextW(LONG(scan<<16),name,64)?Utf8(name):std::to_string(vk);}
inline bool ValidMenuValue(const ToolMenuControl& d,const std::wstring& value){
 if(d.kind==MenuControlKind::Text||d.kind==MenuControlKind::Clear)return value.empty()||!QuickChatText(value).empty();
 if(d.kind==MenuControlKind::Key){if(value==L"-1")return true;if(value.empty()||value.size()>4||value.find_first_not_of(L"0123456789")!=std::wstring::npos)return false;
 auto code=wcstol(value.c_str(),nullptr,10);return code<=0x30b&&EngineKeyToVirtualKey(int(code))&&EngineKeyToVirtualKey(int(code))!=VK_ESCAPE;}
 if(value.empty()||value.size()>16)return false;wchar_t* end=nullptr;auto number=wcstod(value.c_str(),&end);
 return end!=value.c_str()&&!*end&&std::isfinite(number)&&number>=d.minimum&&number<=d.maximum&&
 ((d.kind!=MenuControlKind::Check&&d.kind!=MenuControlKind::Choice)||number==std::floor(number));}
class ToolMenuOverlay{public:
 static std::string ControlHelp(const ToolMenuControl& d){return toolEnglish.load()?EnglishControlHelp(d):MenuControlHelp(d);}
 std::string ControlHint(const ToolMenuControl& d)const{return ControlHelp(d)+(controlAvailable&&!controlAvailable(d)?(toolEnglish.load()?"\nNot available on this game version yet.":Utf8(L"\nCh\u01b0a h\u1ed7 tr\u1ee3 tr\u00ean phi\u00ean b\u1ea3n game n\u00e0y.")):"");}
 std::string HelpAt(POINT p,int w,int h,const Config& c)const{
  int hit=Hit(p,w,h);
  if(hit==-5)return toolEnglish.load()?"Macro recorder\nShow or hide Record, Stop, Run and Script buttons.":Utf8(L"Ghi macro\nM\u1edf ho\u1eb7c \u0111\u00f3ng c\u00e1c n\u00fat Ghi, D\u1eebng, Ch\u1ea1y v\u00e0 Script.");
  if(hit<=-6&&hit>=-9){static const wchar_t* keys[]={L"play",L"stop",L"record",L"edit"};ToolMenuControl d{};d.kind=MenuControlKind::Clear;d.section=L"Macro";d.key=keys[-hit-6];d.label="Macro";return ControlHint(d);}
  if(hit==-1)return toolEnglish.load()?"Tools menu\nOpen or close the in-game tools menu.":Utf8(L"Menu công cụ\nMở hoặc đóng menu công cụ trong trận.");
  if(hit==-2)return toolEnglish.load()?"Close menu\nClose the tools menu and return to the match.":Utf8(L"Đóng menu\nĐóng bảng công cụ và quay lại trận.");
  if(hit>=500&&hit<508){static const wchar_t* tabs[]={L"Tổng quát\nChỉnh hiển thị và các thao tác hỗ trợ trong trận.",L"Chat nhanh\nGán phím và nội dung tin nhắn gửi nhanh.",L"Phím tắt\nGán phím kỹ năng, vật phẩm và chọn tướng.",L"Phím tắt khác\nGán phím mở menu, thu gọn bảng và tạm tắt thiết lập.",L"Đồ họa\nChỉnh hiển thị giao diện, màu sắc và đồng bộ khung hình.",L"MTH\nChọn chế độ và gán thao tác hỗ trợ MTH.",L"Tự động kỹ năng\nChỉnh phím chạy/dừng, độ trễ và phạm vi tìm mục tiêu."};return toolEnglish.load()?ToolTabName(hit-500)+"\nOpen this settings page.":hit==507?Utf8(L"Ngôn ngữ\nChọn ngôn ngữ cho giao diện công cụ."):Utf8(tabs[hit-500]);}
  if(hit>=0&&hit<int(controls.size()))return ControlHint(kToolMenuControls[hit])+(Enabled(c,size_t(hit))||!kToolMenuControls[hit].supported?"":(toolEnglish.load()?"\nDisabled: enable the parent group first.":Utf8(L"\nĐang bị khóa: bật nhóm tùy chọn cha trước.")));
  if(visible&&open&&dropdown<0)for(size_t i=0;i<controls.size();++i){const auto& d=kToolMenuControls[i];if(d.page==page&&d.kind!=MenuControlKind::Label&&*d.label&&FromPixels(d.labelRect).Hit(p,w,h))return ControlHint(d);}
  return {};
 }

 struct HelpLayout{std::string text;float width,height;};
 static HelpLayout LayoutHelp(const std::string& text){
  auto source=Wide(text);std::wstring wrapped;size_t longest=0,lines=0,start=0;
  do{auto end=source.find(L'\n',start);auto line=source.substr(start,end==std::wstring::npos?end:end-start);
   do{size_t count=std::min(size_t(38),line.size());if(line.size()>38){auto space=line.rfind(L' ',count);if(space!=std::wstring::npos&&space>0)count=space;}
    if(lines++)wrapped+=L'\n';wrapped+=line.substr(0,count);longest=std::max(longest,count);line.erase(0,count);while(!line.empty()&&line.front()==L' ')line.erase(0,1);
   }while(!line.empty());if(end==std::wstring::npos)break;start=end+1;
  }while(true);
  return {Utf8(wrapped),std::clamp(float(longest)*.0039f+.016f,.10f,.19f),float(lines)*.016f+.016f};
 }
 void* helpFrame=nullptr,*helpText=nullptr;bool helpShown=false;std::string helpCandidate,helpApplied;ULONGLONG helpSince=0;float helpX=-1,helpY=-1,helpWidth=.19f,helpHeight=.05f;
 void UpdateHelp(POINT p,int w,int h,bool foreground,const std::string& text){
  if(!initialized||!helpFrame)return;auto now=GetTickCount64();auto candidate=foreground&&visible&&w>0&&h>0?text:std::string{};
  if(candidate!=helpCandidate){helpCandidate=candidate;helpSince=now;}
  bool show=!candidate.empty()&&now-helpSince>=300;
  if(show){if(candidate!=helpApplied){auto layout=LayoutHelp(candidate);helpWidth=layout.width;helpHeight=layout.height;
    api.setWidth(helpFrame,helpWidth);api.setHeight(helpFrame,helpHeight);api.setWidth(helpText,helpWidth-.016f);api.setHeight(helpText,helpHeight-.016f);
    auto split=layout.text.find('\n');auto display="|cffffcc00"+layout.text.substr(0,split)+"|r";
    if(split!=std::string::npos)display+="\n|cffffffff"+layout.text.substr(split+1)+"|r";
    api.setText(helpText,display.c_str());helpApplied=candidate;helpX=helpY=-1;}
   float x=std::clamp(float(p.x)*.8f/w+.012f,.004f,.8f-helpWidth-.004f),y=std::clamp(float(h-p.y)*.6f/h+.012f,.004f,std::max(.004f,.6f-helpHeight-.004f));
   if(x!=helpX||y!=helpY){api.setAbsPoint(helpFrame,0,x,y+helpHeight,1);api.setAbsPoint(helpText,0,x+.008f,y+helpHeight-.008f,1);helpX=x;helpY=y;}}
  if(show!=helpShown){if(show)NativeFrameShow(helpFrame);else NativeFrameHide(helpFrame);helpShown=show;}
 }

 static constexpr ToolMenuRect buttonRect{.797f,.155f,.022f,.022f},panelRect{.700f,.1725f,.600f,.375f},closeRect{.690f,.526f,.012f,.012f};
 static constexpr ToolMenuRect macroRect{.797f,.182f,.022f,.022f};
 static ToolMenuRect MacroActionRect(size_t i){return {.770f-float(3-i)*.052f,.182f,.048f,.022f};}
 static const wchar_t* MacroCommand(size_t i){static const wchar_t* commands[]={L"#macro play",L"#macro stop",L"#macro record",L"#macro edit"};return commands[i];}
 static std::string MacroLabel(size_t i){static const wchar_t* vi[]={L"Ch\u1ea1y",L"D\u1eebng",L"Ghi",L"Script"};static const char* en[]={"Run","Stop","Record","Script"};return toolEnglish.load()?en[i]:Utf8(vi[i]);}
 void* macroButton=nullptr,*macroIcon=nullptr;std::array<void*,4> macroActions{},macroLabels{};
 bool macroAvailable=false,macroOpen=false,appliedMacroOpen=true;int appliedMacroLanguage=-1;
 inline static LONG viewportWidth=0,viewportHeight=0;
 static void SetViewport(LONG w,LONG h){viewportWidth=w;viewportHeight=h;}
 static ToolMenuRect FromPixels(MenuPixelRect r){ToolMenuRect result{panelRect.right-panelRect.width+(r.x+r.width)*panelRect.width/1420.f,
 panelRect.bottom+panelRect.height-(r.y+r.height)*panelRect.height/625.f,r.width*panelRect.width/1420.f,r.height*panelRect.height/625.f};
 if(r.width==18 && r.height==18 && viewportWidth>0 && viewportHeight>0){
  float side=std::max(1.f,std::round(std::min(result.width*viewportWidth/.8f,result.height*viewportHeight/.6f)));
  float left=std::round((result.right-result.width)*viewportWidth/.8f);
  float top=std::round((.6f-result.bottom-result.height)*viewportHeight/.6f);
  result={(left+side)*.8f/viewportWidth,.6f-(top+side)*.6f/viewportHeight,side*.8f/viewportWidth,side*.6f/viewportHeight};
 }return result;}
 static ToolMenuRect TabRect(size_t i){return FromPixels({70,55+float(i)*59,252,42});}
 static ToolMenuRect RowRect(size_t i){return FromPixels(kToolMenuControls[i].rect);}
 static ToolMenuRect ValueRect(size_t i){auto r=RowRect(i);
  if(kToolMenuControls[i].kind==MenuControlKind::Text){
   // Text is positioned independently of the editbox backdrop. Keep the
   // font bounds inside its bevel instead of anchoring on the outer edge.
   constexpr float horizontal=.006f,vertical=.002f;
   r.right-=horizontal;r.width-=2*horizontal;r.bottom+=vertical;r.height-=2*vertical;
  }return r;}
 struct ControlUI{void *frame=nullptr,*value=nullptr,*label=nullptr,*tick=nullptr,*base=nullptr;int appliedEnabled=-1;std::string applied,appliedLabel;};
 std::array<void*,3> bossFrames{},bossFonts{};std::array<std::string,3> bossText{};std::array<bool,3> bossShown{};
 void UpdateBoss(const std::array<uint32_t,3>& remaining,bool enabled){if(!initialized)return;
  for(size_t i=0;i<3;++i){bool show=enabled&&remaining[i]>0;if(show){auto text="|cffffcc00"+std::to_string(remaining[i])+"|r";if(text!=bossText[i]){api.setText(bossFonts[i],text.c_str());bossText[i]=text;}}
   if(show!=bossShown[i]){if(show)NativeFrameShow(bossFrames[i]);else NativeFrameHide(bossFrames[i]);bossShown[i]=show;}}}

 void* independentHost=nullptr;
 ExtendedFrameApi api{};void (__thiscall* tint)(void*,const uint32_t*)=nullptr;uintptr_t parent=0;void *button=nullptr,*menuIcon=nullptr,*panel=nullptr,*close=nullptr;
 std::array<void*,8> tabs{},tabLabels{};std::array<void*,8> tabGlows{};void* popupHost=nullptr;void* popupTexture=nullptr;std::array<void*,3> grids{};std::array<ControlUI,kToolMenuControls.size()> controls{};
 std::array<void*,20> choices{},choiceLabels{};
 bool (*controlAvailable)(const ToolMenuControl&)=nullptr;
 bool initialized=false,failed=false,visible=false,open=false,appliedOpen=true,menuPressedVisual=false;
 bool appliedEnglish=false;int pressed=-3,page=0,appliedPage=-1,editing=-1,dropdown=-1,appliedDropdown=-2;std::wstring editBuffer;
 LONG appliedViewportWidth=0,appliedViewportHeight=0;
 ULONGLONG menuPressedUntil=0,nextRefresh=0;const char* initStage="not started";
 void Reset(){macroButton=macroIcon=nullptr;macroActions={};macroLabels={};macroAvailable=macroOpen=false;appliedMacroOpen=true;appliedMacroLanguage=-1;bossFrames={};bossFonts={};bossText={};bossShown={};independentHost=nullptr;helpFrame=helpText=nullptr;helpShown=false;helpCandidate.clear();helpApplied.clear();helpSince=0;helpX=helpY=-1;parent=0;button=menuIcon=panel=close=nullptr;tabs={};tabLabels={};tabGlows={};popupHost=popupTexture=nullptr;grids={};controls={};choices={};choiceLabels={};
 initialized=failed=visible=open=menuPressedVisual=false;appliedOpen=true;pressed=-3;page=0;appliedPage=editing=dropdown=-1;appliedDropdown=-2;editBuffer.clear();menuPressedUntil=nextRefresh=0;initStage="not started";appliedViewportWidth=appliedViewportHeight=0;}
 void CancelEdit(){editing=dropdown=-1;editBuffer.clear();nextRefresh=0;}
 void Toggle(){open=!open;CancelEdit();if(open)page=0;}
 void MenuPressVisual(bool down){if(menuPressedVisual==down)return;menuPressedVisual=down;if(!button||!menuIcon||!api.setWidth||!api.setHeight||!api.setRelPoint)return;
 float size=down?.0198f:.022f;api.setWidth(menuIcon,size);api.setHeight(menuIcon,size);api.setRelPoint(menuIcon,4,button,4,down?.001f:0.f,down?-.001f:0.f,1);}
 void* Font(void* owner,float height,const char* text){auto font=NativeCreateFontString(api,owner,height,"",kNativeOverlayLayer);if(!font)return nullptr;
 if(!api.setFont(font,"fonts_VN.ttf",height,1))api.setFont(font,"WHTUI_FRIZQT__.ttf",height,1);api.setText(font,text);return font;}
 void* Texture(void* owner,const char* path,float w,float h,int layer=kNativeOverlayLayer){auto memory=api.alloc(nativeFrameLayout.textureSize,"CSimpleTexture",1,0);if(!memory)return nullptr;
 auto texture=api.textureCtor(memory,owner,layer,1);if(!texture)return nullptr;api.setWidth(texture,w);api.setHeight(texture,h);api.setTexture(texture,path,0);return texture;}
 void* Frame(uintptr_t game,void* owner,ToolMenuRect r,uint32_t level=8,bool detached=false){
 // Game+60A120 assigns parent level + 1 during construction, before Show.
 // Reject an invalid parent before the ctor can register a level-10 frame.
 if(level>9 || (!owner&&!detached) || (owner&&*reinterpret_cast<uint32_t*>(static_cast<char*>(owner)+nativeFrameLayout.level)>=9))return nullptr;
 auto memory=api.alloc(nativeFrameLayout.frameSize,"CSimpleFrame",1,0);if(!memory)return nullptr;
 auto frame=api.frameCtor(memory,owner);if(!frame)return nullptr;NativeFrameHide(frame);api.setWidth(frame,r.width);api.setHeight(frame,r.height);api.setAbsPoint(frame,0,r.right-r.width,r.bottom+r.height,1);
 if(!NativeBringToolFrameToFront(frame,game))return nullptr;NativeFrameHide(frame);*reinterpret_cast<uint32_t*>(reinterpret_cast<uintptr_t>(frame)+nativeFrameLayout.level)=level;return frame;}
 void Style(void* frame,bool edit=false){NativeSetFrameBackdropEx(api,frame,"UI\\Widgets\\EscMenu\\Human\\human-options-menu-background.blp",
 edit?"UI\\Widgets\\EscMenu\\Human\\editbox-border.blp":"UI\\Widgets\\EscMenu\\Human\\human-options-button-border-up.blp",edit?.005f:.010f,.002f);}
 void* TextIn(void* owner,ToolMenuRect r,const char* text,bool centre=false,float height=.0115f){auto font=Font(owner,height,text);if(!font)return nullptr;
 api.setWidth(font,r.width);api.setHeight(font,r.height);api.setTextAlign(font,centre?2:1);api.setAbsPoint(font,0,r.right-r.width,r.bottom+r.height,1);return font;}
 bool Init(uintptr_t game,uintptr_t owner,const ExtendedFrameApi* supplied=nullptr){if(supplied)api=*supplied;else {auto storm=GetModuleHandleW(L"Storm.dll");if(!storm)return false;const auto& o=ActiveGameOffsets();
 api.alloc=reinterpret_cast<decltype(api.alloc)>(GetProcAddress(storm,reinterpret_cast<LPCSTR>(401)));
#define WHT_TOOL_API(m) api.m=reinterpret_cast<decltype(api.m)>(game+o.m)
 WHT_TOOL_API(frameCtor);WHT_TOOL_API(backdropCtor);WHT_TOOL_API(setBackdrop);WHT_TOOL_API(stringAssign);WHT_TOOL_API(fontCtor);WHT_TOOL_API(setFont);WHT_TOOL_API(setTextAlign);WHT_TOOL_API(setText);
 WHT_TOOL_API(textureCtor);WHT_TOOL_API(setTexture);WHT_TOOL_API(setWidth);WHT_TOOL_API(setHeight);WHT_TOOL_API(setAbsPoint);WHT_TOOL_API(setRelPoint);
#undef WHT_TOOL_API
 tint=reinterpret_cast<decltype(tint)>(game+o.setTextureColor);}
 if(!api.alloc||!HudActionToolbar::SafeIsExecutable(reinterpret_cast<uintptr_t>(api.frameCtor)))return false;
 // Native SetParent supports null: a root host has no console visibility
 // dependency. All menu children keep the verified 0..9 frame levels.
 initStage="independent menu host";independentHost=Frame(game,nullptr,{.8f,0,.8f,.6f},6,true);if(!independentHost)return false;
 owner=reinterpret_cast<uintptr_t>(independentHost);NativeFrameShow(independentHost);
 initStage="menu button";button=Frame(game,reinterpret_cast<void*>(owner),buttonRect);if(!button)return false;menuIcon=Texture(button,"BTNWHT_Menu.blp",.022f,.022f);if(!menuIcon)return false;api.setRelPoint(menuIcon,4,button,4,0,0,1);
 initStage="macro toolbar";macroButton=Frame(game,reinterpret_cast<void*>(owner),macroRect);if(!macroButton)return false;Style(macroButton);
 macroIcon=Texture(macroButton,"BTNWHT_Rec.blp",macroRect.width,macroRect.height);if(!macroIcon)return false;api.setRelPoint(macroIcon,4,macroButton,4,0,0,1);
 for(size_t i=0;i<macroActions.size();++i){auto r=MacroActionRect(i);macroActions[i]=Frame(game,reinterpret_cast<void*>(owner),r);if(!macroActions[i])return false;Style(macroActions[i]);macroLabels[i]=TextIn(macroActions[i],r,"",true,.0105f);if(!macroLabels[i])return false;}
 initStage="panel";panel=Frame(game,reinterpret_cast<void*>(owner),panelRect,7);if(!panel)return false;
 NativeSetFrameBackdropEx(api,panel,nullptr,"UI\\Widgets\\EscMenu\\NightElf\\nightelf-options-menu-border.blp",.027f,.008f);
 auto background=Texture(panel,"WHTUI_GreyBackground.tga",panelRect.width-.018f,panelRect.height-.018f,0);if(!background)return false;api.setRelPoint(background,0,panel,0,.009f,-.009f,1);
 // Original WHT assets include transparent canvas margins around the grids.
 // Fit their non-transparent content to the reference, not the entire canvas.
 for(size_t i=0;i<grids.size();++i){const bool inventory=i==2;
 auto r=FromPixels(inventory?MenuPixelRect{468,387,187,194}:MenuPixelRect{i==0?375.f:905.f,95,440,234});
 grids[i]=Frame(game,panel,r);if(!grids[i])return false;
 const float canvas=inventory?256.f:512.f,cropW=inventory?158.f:366.f,cropH=inventory?233.f:278.f;
 auto texture=Texture(grids[i],inventory?"InventoryHuman.tga":"CommandCardHuman.tga",r.width*canvas/cropW,r.height*canvas/cropH,0);
 if(!texture)return false;api.setRelPoint(texture,0,grids[i],0,-r.width*(inventory?49.f:72.f)/cropW,r.height*(inventory?11.f:117.f)/cropH,1);}
 close=Frame(game,panel,closeRect);if(!close||!TextIn(close,closeRect,"|cffffcc00X|r",true,.010f))return false;
 initStage="tabs";for(size_t i=0;i<tabs.size();++i){tabs[i]=Frame(game,panel,TabRect(i));if(!tabs[i])return false;Style(tabs[i]);tabGlows[i]=Texture(tabs[i],"UI\\Widgets\\EscMenu\\Human\\human-options-menu-background.blp",TabRect(i).width-.008f,TabRect(i).height-.004f,0);if(!tabGlows[i])return false;api.setRelPoint(tabGlows[i],4,tabs[i],4,0,0,1);auto title=std::string("|cffffcc00")+ToolTabName(i)+"|r";
 tabLabels[i]=TextIn(tabs[i],TabRect(i),title.c_str(),true);if(!tabLabels[i])return false;}
 initStage="controls";for(size_t i=0;i<controls.size();++i){auto& ui=controls[i];const auto& d=kToolMenuControls[i];auto r=RowRect(i);ui.frame=Frame(game,panel,r);if(!ui.frame)return false;
 if(d.kind==MenuControlKind::Check){auto base=Texture(ui.frame,"UI\\Widgets\\EscMenu\\Human\\checkbox-background.blp",r.width,r.height);ui.tick=Texture(ui.frame,"Transparent.tga",r.width,r.height);if(!base||!ui.tick)return false;ui.base=base;api.setRelPoint(base,4,ui.frame,4,0,0,1);api.setRelPoint(ui.tick,4,ui.frame,4,0,0,1);}
 else if(d.kind!=MenuControlKind::Label){const bool slot=d.kind==MenuControlKind::Key&&std::wstring(d.section).rfind(L"SlotHotkey",0)==0;
 if(!slot)Style(ui.frame,d.kind==MenuControlKind::Text||d.kind==MenuControlKind::Number||d.kind==MenuControlKind::Choice);ui.value=TextIn(ui.frame,ValueRect(i),"",d.kind!=MenuControlKind::Text,.0105f);if(!ui.value)return false;}
 if(*d.label){ui.label=TextIn(ui.frame,FromPixels(d.labelRect),"",d.kind==MenuControlKind::Clear);if(!ui.label)return false;}}
 initStage="dropdown background";popupHost=Frame(game,panel,ChoiceRect(0),9);if(!popupHost)return false;popupTexture=Texture(popupHost,"UI\\Widgets\\EscMenu\\Human\\human-options-menu-background.blp",.04f,.04f,0);if(!popupTexture)return false;api.setRelPoint(popupTexture,0,popupHost,0,0,0,1);
 // Popup and rows are siblings. A native child of a level-9 host inherits
 // level 10, outside the engine's ten frame buckets, and fails initialization.
 initStage="dropdown rows";for(size_t i=0;i<choices.size();++i){choices[i]=Frame(game,panel,ChoiceRect(i),9);if(!choices[i])return false;Style(choices[i],true);choiceLabels[i]=TextIn(choices[i],ChoiceRect(i),"",true,.010f);if(!choiceLabels[i])return false;}initStage="tooltip";helpFrame=Frame(game,reinterpret_cast<void*>(owner),{.25f,.20f,.24f,.09f},9);if(!helpFrame)return false;
 NativeSetFrameBackdropEx(api,helpFrame,"UI\\Widgets\\ToolTips\\Human\\human-tooltip-background.blp","UI\\Widgets\\ToolTips\\Human\\human-tooltip-border.blp",.008f,.002f);helpText=TextIn(helpFrame,{.242f,.208f,.224f,.074f},"",false,.0135f);if(!helpText)return false;
 initStage="boss timers";const std::array<ToolMenuRect,3> bossRects{{{.220f,.061f,.036f,.022f},{.136f,.009f,.036f,.022f},{.220f,.127f,.036f,.022f}}};
 for(size_t i=0;i<3;++i){bossFrames[i]=Frame(game,reinterpret_cast<void*>(owner),bossRects[i],9);if(!bossFrames[i])return false;bossFonts[i]=TextIn(bossFrames[i],bossRects[i],"",true,.012f);if(!bossFonts[i])return false;}
 initStage="ready";return true;}
 static std::wstring ReadValue(const Config& c,size_t i){const auto& d=kToolMenuControls[i];return MenuRead(c,d.section,d.key,d.fallback);}
 static bool Enabled(const Config& c,size_t i){const auto& d=kToolMenuControls[i];if(!d.supported)return false;
  std::wstring dep=d.dependency;if(dep.empty())return true;
  auto term=[&](const std::wstring& condition){auto at=condition.find(L'|');if(at==std::wstring::npos)return false;
   auto section=condition.substr(0,at),key=condition.substr(at+1);const wchar_t* fallback=L"0";
   for(const auto& parentControl:kToolMenuControls)if(section==parentControl.section && key==parentControl.key){fallback=parentControl.fallback;break;}
   return MenuRead(c,section.c_str(),key.c_str(),fallback)==L"1";};
  size_t start=0;do{auto end=dep.find(L"||",start);auto branch=dep.substr(start,end==std::wstring::npos?end:end-start);
   bool on=true;size_t item=0;do{auto next=branch.find(L"&&",item);on=on&&term(branch.substr(item,next==std::wstring::npos?next:next-item));
    if(next==std::wstring::npos)break;item=next+2;}while(true);
   if(on)return true;if(end==std::wstring::npos)break;start=end+2;}while(true);return false;
 }
 static std::string ChoiceName(const ToolMenuControl& d,int n){
  if(!wcscmp(d.key,L"ModeCast")){static const char* names[]={"Quick Cast","Smart Cast","Quick Cast Old"};return n>=0&&n<3?names[n]:names[0];}
  return std::to_string(n);
 }
 static std::string Display(const ToolMenuControl& d,const std::wstring& value){if(d.kind==MenuControlKind::Key)return MenuKeyName(value);
 if(d.kind==MenuControlKind::Choice&&!wcscmp(d.key,L"ModeCast"))return ChoiceName(d,int(wcstol(value.c_str(),nullptr,10)))+"  v";if(d.kind==MenuControlKind::Choice)return Utf8(value)+(d.page==0?" sec  v":" Count Shift  v");if(d.kind==MenuControlKind::Clear)return {};return Utf8(value);}
 ToolMenuRect ChoiceRect(size_t n)const{auto source=dropdown>=0?RowRect(size_t(dropdown)):FromPixels({685,337,104,22});
 size_t count=dropdown>=0?size_t(kToolMenuControls[dropdown].maximum-kToolMenuControls[dropdown].minimum+1):20u;
 float top=std::min(panelRect.bottom+panelRect.height-.015f,std::max(source.bottom,panelRect.bottom+.012f+float(count)*.016f));return {source.right,top-float(n+1)*.016f,source.width,.016f};}
 void Update(uintptr_t game,uintptr_t owner,const Config& c,bool allowed,const ExtendedFrameApi* supplied=nullptr){if(parent!=owner){Reset();parent=owner;}if(!game||!owner)return;
 if(!initialized&&!failed&&allowed){initialized=Init(game,owner,supplied);failed=!initialized;}if(!initialized){NativeFrameHide(button);NativeFrameHide(panel);for(auto& ui:controls)NativeFrameHide(ui.frame);return;}
 if(viewportWidth!=appliedViewportWidth || viewportHeight!=appliedViewportHeight){
  for(size_t i=0;i<controls.size();++i){auto& d=kToolMenuControls[i];if(d.kind!=MenuControlKind::Check || d.rect.width!=18 || d.rect.height!=18)continue;
   auto r=RowRect(i);auto& ui=controls[i];api.setAbsPoint(ui.frame,0,r.right-r.width,r.bottom+r.height,1);
   for(auto part:{ui.frame,ui.base,ui.tick}){api.setWidth(part,r.width);api.setHeight(part,r.height);}}
  appliedViewportWidth=viewportWidth;appliedViewportHeight=viewportHeight;
 }
 if(visible!=allowed){visible=allowed;if(allowed){NativeFrameShow(button);NativeFrameShow(macroButton);}else{NativeFrameHide(button);NativeFrameHide(macroButton);macroOpen=false;open=false;pressed=-3;CancelEdit();menuPressedUntil=0;MenuPressVisual(false);}}
 macroAvailable=visible&&c.macroEnabled&&c.macroShowButton;if(macroAvailable)NativeFrameShow(macroButton);else{NativeFrameHide(macroButton);macroOpen=false;}
 bool macroShow=macroAvailable&&macroOpen;
 if(macroShow!=appliedMacroOpen||appliedMacroLanguage!=int(c.english)){
  for(size_t i=0;i<macroActions.size();++i){auto text="|cffffcc00"+MacroLabel(i)+"|r";api.setText(macroLabels[i],text.c_str());if(macroShow)NativeFrameShow(macroActions[i]);else NativeFrameHide(macroActions[i]);}
  appliedMacroOpen=macroShow;appliedMacroLanguage=int(c.english);
 }
 if((editing>=0&&!Enabled(c,size_t(editing))) || (dropdown>=0&&!Enabled(c,size_t(dropdown))))CancelEdit();
 MenuPressVisual(visible&&(pressed==-1||GetTickCount64()<menuPressedUntil));bool show=visible&&open;
 if(c.english!=appliedEnglish){appliedEnglish=c.english;appliedPage=-1;helpApplied.clear();nextRefresh=0;} if(show!=appliedOpen||page!=appliedPage){if(show){NativeFrameShow(panel);NativeFrameShow(close);for(auto tab:tabs)NativeFrameShow(tab);}else{for(auto tab:tabs)NativeFrameHide(tab);NativeFrameHide(close);NativeFrameHide(panel);}
 for(size_t i=0;i<controls.size();++i){if(show&&kToolMenuControls[i].page==page)NativeFrameShow(controls[i].frame);else NativeFrameHide(controls[i].frame);}for(auto grid:grids){if(show&&page==2)NativeFrameShow(grid);else NativeFrameHide(grid);}
 for(size_t i=0;i<tabs.size();++i){api.setTexture(tabGlows[i],int(i)==page?"UI\\Widgets\\EscMenu\\Human\\human-options-menu-background.blp":"Transparent.tga",0);
 auto title=std::string(int(i)==page?"|cffffffff":"|cffffcc00")+ToolTabName(i)+"|r";api.setText(tabLabels[i],title.c_str());}
 appliedOpen=show;appliedPage=page;nextRefresh=0;}
 if(show&&GetTickCount64()>=nextRefresh){nextRefresh=GetTickCount64()+1000;for(size_t i=0;i<controls.size();++i){const auto& d=kToolMenuControls[i];if(d.page!=page)continue;auto& ui=controls[i];bool enabled=Enabled(c,i)&&(!controlAvailable||controlAvailable(d));auto colour=enabled?"|cffffcc00":"|cff888888";
 if(ui.base && tint && ui.appliedEnabled!=int(enabled)){uint32_t tintColour=enabled?0xffffffff:0xff666666;tint(ui.base,&tintColour);tint(ui.tick,&tintColour);ui.appliedEnabled=int(enabled);}
 if(ui.label){auto text=std::string(colour)+(toolEnglish.load()?EnglishControlLabel(d):std::string(d.label))+"|r";if(text!=ui.appliedLabel){api.setText(ui.label,text.c_str());ui.appliedLabel=text;}}
 auto value=ReadValue(c,i);if(std::wstring(d.section)==L"Language")value=(std::wstring(d.key)==L"English"?c.english:!c.english)?L"1":L"0";std::string display;if(d.kind==MenuControlKind::Check){display=value==L"1"?"1":"0";if(display!=ui.applied)api.setTexture(ui.tick,value==L"1"?"UI\\Widgets\\EscMenu\\Human\\checkbox-check.blp":"Transparent.tga",0);}
 else if(ui.value){display=std::string(colour)+Display(d,editing==int(i)?editBuffer:value)+(editing==int(i)?"_":"")+"|r";if(display!=ui.applied)api.setText(ui.value,display.c_str());}ui.applied=display;}}
 const int popup=show?dropdown:-1;if(popup!=appliedDropdown){appliedDropdown=popup;
 if(popup>=0){auto first=ChoiceRect(0);auto last=ChoiceRect(size_t(kToolMenuControls[popup].maximum-kToolMenuControls[popup].minimum));
 float height=first.bottom+first.height-last.bottom;
 api.setAbsPoint(popupHost,0,first.right-first.width,last.bottom+height,1);api.setWidth(popupHost,first.width);api.setHeight(popupHost,height);
 api.setWidth(popupTexture,first.width);api.setHeight(popupTexture,height);NativeFrameShow(popupHost);
 }else NativeFrameHide(popupHost);
 for(size_t i=0;i<choices.size();++i){if(show&&dropdown>=0&&i<size_t(kToolMenuControls[dropdown].maximum-kToolMenuControls[dropdown].minimum+1)){auto r=ChoiceRect(i);
 api.setAbsPoint(choices[i],0,r.right-r.width,r.bottom+r.height,1);api.setWidth(choices[i],r.width);api.setAbsPoint(choiceLabels[i],0,r.right-r.width,r.bottom+r.height,1);api.setWidth(choiceLabels[i],r.width);
 auto text="|cffffcc00"+ChoiceName(kToolMenuControls[dropdown],int(kToolMenuControls[dropdown].minimum)+int(i))+"|r";api.setText(choiceLabels[i],text.c_str());NativeFrameShow(choices[i]);}else NativeFrameHide(choices[i]);}}}
 int Hit(POINT p,int w,int h)const{if(!visible)return -3;if(buttonRect.Hit(p,w,h))return -1;if(macroAvailable&&macroRect.Hit(p,w,h))return -5;if(macroAvailable&&macroOpen)for(size_t i=0;i<4;++i)if(MacroActionRect(i).Hit(p,w,h))return -6-int(i);if(!open)return -3;
 if(dropdown>=0)for(int i=0;i<=int(kToolMenuControls[dropdown].maximum-kToolMenuControls[dropdown].minimum);++i)if(ChoiceRect(i).Hit(p,w,h))return 1000+i;
 if(closeRect.Hit(p,w,h))return -2;for(size_t i=0;i<tabs.size();++i)if(TabRect(i).Hit(p,w,h))return 500+int(i);
 if(dropdown>=0)return panelRect.Hit(p,w,h)?-4:-3;
 for(size_t i=controls.size();i-->0;){const auto& d=kToolMenuControls[i];if(d.page==page&&d.kind!=MenuControlKind::Label&&RowRect(i).Hit(p,w,h))return int(i);}return panelRect.Hit(p,w,h)?-4:-3;}
 template<class Queue> static void QueueValue(Queue queue,size_t i,const wchar_t* op,const std::wstring& value=L""){auto cmd=L"#toolui "+std::to_wstring(i)+L" "+op+(value.empty()?L"":L" "+value);queue(cmd.c_str());}
 template<class Queue> bool Mouse(unsigned message,POINT p,int w,int h,Queue queue,const Config* c=nullptr){int hit=Hit(p,w,h);
 if(message==WM_LBUTTONDOWN||message==WM_LBUTTONDBLCLK){if(hit==-3&&(editing>=0||dropdown>=0))CancelEdit();pressed=hit;if(hit==-1)menuPressedUntil=GetTickCount64()+120;return hit!=-3;}if(message!=WM_LBUTTONUP)return false;
 int down=pressed;pressed=-3;if(down==-1&&hit!=-1)menuPressedUntil=0;if(down==-3)return hit!=-3;if(down!=hit)return true;
 if(hit==-5)macroOpen=!macroOpen;else if(hit<=-6&&hit>=-9){CancelEdit();open=false;queue(MacroCommand(size_t(-hit-6)));}else if(hit==-1)Toggle();else if(hit==-2){open=false;CancelEdit();}else if(hit>=1000){int selected=dropdown;CancelEdit();if(selected>=0)QueueValue(queue,size_t(selected),L"set",std::to_wstring(int(kToolMenuControls[selected].minimum)+hit-1000));}
 else if(hit>=500){page=hit-500;CancelEdit();}else if(hit>=0){const auto& d=kToolMenuControls[hit];if(!d.supported||(controlAvailable&&!controlAvailable(d))){QueueValue(queue,size_t(hit),L"unsupported");return true;}if(c&&!Enabled(*c,size_t(hit)))return true;
 if(d.kind==MenuControlKind::Check)QueueValue(queue,size_t(hit),L"toggle");else if(d.kind==MenuControlKind::Clear)QueueValue(queue,size_t(hit),L"clear");else if(d.kind==MenuControlKind::Choice){editing=-1;dropdown=dropdown==hit?-1:hit;nextRefresh=0;}
 else{CancelEdit();editing=hit;if(c)editBuffer=ReadValue(*c,size_t(hit));nextRefresh=0;}}else if(hit==-4)CancelEdit();return true;}
 template<class Queue> bool Key(unsigned key,unsigned scan,Queue queue){if(!visible||!open||editing<0)return false;size_t index=size_t(editing);const auto& d=kToolMenuControls[index];
 if(key==VK_ESCAPE){CancelEdit();return true;}if(d.kind==MenuControlKind::Key){if(key==VK_SHIFT||key==VK_CONTROL||key==VK_MENU||key==VK_LWIN||key==VK_RWIN)return true;
 auto value=key==VK_BACK?L"-1":std::to_wstring(MenuEngineKey(key));if(ValidMenuValue(d,value)){QueueValue(queue,index,L"set",value);CancelEdit();}return true;}
 if(key==VK_RETURN){if(ValidMenuValue(d,editBuffer)){QueueValue(queue,index,L"set",editBuffer);CancelEdit();}return true;}
 if(key==VK_BACK){if(!editBuffer.empty()){editBuffer.pop_back();if(!editBuffer.empty()&&editBuffer.back()>=0xd800&&editBuffer.back()<=0xdbff)editBuffer.pop_back();}nextRefresh=0;return true;}
 BYTE state[256]{};WCHAR chars[8]{};if(GetKeyboardState(state)){int n=ToUnicode(key,scan,state,chars,8,4);for(int i=0;i<n&&editBuffer.size()<128;++i)if(chars[i]>=32)editBuffer+=chars[i];}nextRefresh=0;return true;}
 bool Escape(){if(!visible)return false;if(!open){if(!macroOpen)return false;macroOpen=false;pressed=-3;return true;}if(editing>=0||dropdown>=0)CancelEdit();else open=false;pressed=-3;return true;}
};}
