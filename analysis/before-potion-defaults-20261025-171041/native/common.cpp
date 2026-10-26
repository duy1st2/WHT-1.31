#include "common.hpp"
#include "auto_heal.hpp"
#include <bcrypt.h>
#include <fstream>
#include <filesystem>
#include <mutex>
#include <sstream>
#include <cmath>
#include <limits>

namespace wht {
std::wstring Directory(HMODULE m) {
    std::vector<wchar_t> b(32768);
    DWORD n=GetModuleFileNameW(m,b.data(),static_cast<DWORD>(b.size()));
    if(!n||n>=b.size()) return {};
    return std::filesystem::path(std::wstring(b.data(),n)).parent_path().wstring();
}
std::string Utf8(const std::wstring& s) {
    int n=WideCharToMultiByte(CP_UTF8,0,s.data(),static_cast<int>(s.size()),nullptr,0,nullptr,nullptr);
    std::string out(n,'\0'); if(n) WideCharToMultiByte(CP_UTF8,0,s.data(),static_cast<int>(s.size()),out.data(),n,nullptr,nullptr); return out;
}
std::wstring Wide(const std::string& s) {
    int n=MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,s.data(),static_cast<int>(s.size()),nullptr,0);
    std::wstring out(n,L'\0'); if(n) MultiByteToWideChar(CP_UTF8,MB_ERR_INVALID_CHARS,s.data(),static_cast<int>(s.size()),out.data(),n); return out;
}
std::string Sha256(const void* bytes,size_t length) {
    if(length>std::numeric_limits<ULONG>::max()) return {};
    BCRYPT_ALG_HANDLE alg=nullptr; unsigned char hash[32]{};
    if(BCryptOpenAlgorithmProvider(&alg,BCRYPT_SHA256_ALGORITHM,nullptr,0)<0) return {};
    NTSTATUS result=BCryptHash(alg,nullptr,0,(PUCHAR)bytes,static_cast<ULONG>(length),hash,32);
    BCryptCloseAlgorithmProvider(alg,0); if(result<0)return {};
    const char* digits="0123456789ABCDEF";std::string out;
    for(auto c:hash){out+=digits[c>>4];out+=digits[c&15];} return out;
}
std::string HashFile(const std::wstring& path) {
    std::ifstream f(std::filesystem::path(path),std::ios::binary|std::ios::ate);
    if(!f) return {};auto size=f.tellg();if(size<0||size>64*1024*1024)return {};
    std::vector<char> b(static_cast<size_t>(size));f.seekg(0);if(!f.read(b.data(),size))return {};
    return Sha256(b.data(),b.size());
}
static thread_local const Config::Profile* loadingProfile=nullptr;
static std::wstring ProfileName(std::wstring name){for(auto& ch:name)ch=towlower(ch);return name;}
static std::shared_ptr<Config::Profile> ReadProfile(const std::wstring& path){
    auto result=std::make_shared<Config::Profile>();
    std::vector<wchar_t> names(65536),values(65536);
    GetPrivateProfileSectionNamesW(names.data(),DWORD(names.size()),path.c_str());
    for(auto section=names.data();*section;section+=wcslen(section)+1){
        GetPrivateProfileSectionW(section,values.data(),DWORD(values.size()),path.c_str());
        auto& entries=(*result)[ProfileName(section)];
        for(auto item=values.data();*item;item+=wcslen(item)+1){
            auto equal=wcschr(item,L'=');if(!equal)continue;
            std::wstring value=equal+1;
            if(value.size()>=2&&((value.front()==L'\"'&&value.back()==L'\"')||(value.front()==L'\''&&value.back()==L'\'')))value=value.substr(1,value.size()-2);
            entries.emplace(ProfileName(std::wstring(item,equal)),std::move(value));
        }
    }
    return result;
}
static std::wstring Read(const std::wstring& p,const wchar_t* s,const wchar_t* k,const wchar_t* def) {
    if(loadingProfile){auto section=loadingProfile->find(ProfileName(s));if(section==loadingProfile->end())return def;
        auto value=section->second.find(ProfileName(k));return value==section->second.end()?def:value->second;}
    wchar_t b[2048]{};GetPrivateProfileStringW(s,k,def,b,2048,p.c_str());return b;
}
static float Number(const std::wstring& s,float fallback,float lo,float hi) {
    wchar_t* end=nullptr;float f=wcstof(s.c_str(),&end);
    if(end==s.c_str()||*end||!std::isfinite(f)||f<lo||f>hi)return fallback;return f;
}
unsigned EngineKeyToVirtualKey(int key) {
    // Recovered ETS FUN_100524a0. These are ETS/engine codes, not Windows VKs.
    if((key>=0x30&&key<=0x39)||(key>=0x41&&key<=0x5a))return key;
    if(key>=0x300&&key<=0x30b)return VK_F1+key-0x300;
    if(key>=0x101&&key<=0x10a)return VK_NUMPAD0+key-0x101;
    constexpr unsigned punctuation[]={VK_OEM_3,VK_ADD,VK_SUBTRACT,VK_MULTIPLY,VK_DIVIDE,VK_DECIMAL,
        VK_OEM_PLUS,VK_OEM_MINUS,VK_OEM_4,VK_OEM_6,VK_OEM_5,VK_OEM_1,VK_OEM_7,VK_OEM_COMMA,VK_OEM_PERIOD,VK_OEM_2};
    if(key==0x100)return punctuation[0];
    if(key>=0x10b&&key<=0x119)return punctuation[key-0x10a];
    constexpr unsigned special[]={VK_ESCAPE,VK_RETURN,VK_BACK,VK_TAB,VK_LEFT,VK_UP,VK_RIGHT,VK_DOWN,
        VK_INSERT,VK_DELETE,VK_HOME,VK_END,VK_PRIOR,VK_NEXT,VK_CAPITAL,VK_NUMLOCK,VK_SCROLL,VK_PAUSE,VK_SNAPSHOT};
    if(key>=0x200&&key<=0x212)return special[key-0x200];
    if(key==0x20)return VK_SPACE;
    // Modifier-only bindings are deliberately not intercepted.
    return 0;
}
int CommandButtonIndex(int slot) {return slot>=0&&slot<12?(slot%4)*3+slot/4:-1;}
std::string QuickChatText(const std::wstring& text) {
    // ETS sends at most 128 bytes. Reject rather than split a UTF-8 character or
    // silently send a different command. Control characters must not submit extra lines.
    if(text.empty())return {};
    bool visible=false;
    for(size_t i=0;i<text.size();++i){
        const auto ch=text[i];
        if(ch<32||(ch>=0x7f&&ch<=0x9f))return {};
        if(ch!=L' ')visible=true;
        if(ch>=0xd800&&ch<=0xdbff){if(++i>=text.size()||text[i]<0xdc00||text[i]>0xdfff)return {};}
        else if(ch>=0xdc00&&ch<=0xdfff)return {};
    }
    const auto utf8=Utf8(text);
    return visible&&utf8.size()<=128?utf8:std::string{};
}
static unsigned ReadKey(const std::wstring& ini,const wchar_t* section,const wchar_t* key,const wchar_t* fallback=L"-1"){
    const auto text=Read(ini,section,key,fallback);
    wchar_t* end=nullptr;long code=wcstol(text.c_str(),&end,0);
    return end!=text.c_str()&&!*end&&code>=0&&code<=0x30b?EngineKeyToVirtualKey(static_cast<int>(code)):0;
}
template<size_t N> void LoadBindings(const std::wstring& ini,const wchar_t* section,const wchar_t* prefix,const wchar_t* quickPrefix,std::array<Binding,N>& out){
    for(size_t i=0;i<N;++i){
        const auto key=std::wstring(prefix)+std::to_wstring(i);
        out[i].vk=ReadKey(ini,section,key.c_str());
        out[i].enabled=out[i].vk!=0&&Read(ini,section,(key+L"Enabled").c_str(),L"0")==L"1";
        out[i].quickCast=Read(ini,section,(std::wstring(quickPrefix)+std::to_wstring(i)).c_str(),L"0")==L"1";
        out[i].autoTarget=Read(ini,section,(L"AutoTargetCommand"+std::to_wstring(i)).c_str(),L"0")==L"1"||
                          Read(ini,section,(L"AutoTargetCommandShop"+std::to_wstring(i)).c_str(),L"0")==L"1";
        out[i].autoTargetMe=Read(ini,section,(L"AutoTargetCommandMe"+std::to_wstring(i)).c_str(),L"0")==L"1";
    }
}
bool ParseFpsLimit(const std::wstring& text,unsigned& value){
    if(text.empty()||text.size()>4)return false;
    unsigned parsed=0;
    for(const wchar_t ch:text){if(ch<L'0'||ch>L'9')return false;parsed=parsed*10+ch-L'0';}
    if(parsed!=0&&(parsed<60||parsed>1000))return false;
    value=parsed;return true;
}
bool ParseSceneLight(const std::wstring& text,unsigned& value){
    if(text.empty()||text.size()>3)return false;
    unsigned parsed=0;
    for(const wchar_t ch:text){if(ch<L'0'||ch>L'9')return false;parsed=parsed*10+ch-L'0';}
    if(parsed>255)return false;value=parsed;return true;
}
uint32_t ParseHexColor(const std::wstring& text, uint32_t defaultValue){
    if(text.empty())return defaultValue;
    try {
        size_t idx=0;
        unsigned long val=std::stoul(text,&idx,0);
        return static_cast<uint32_t>(val);
    } catch(...) {
        return defaultValue;
    }
}
Config LoadConfig(const std::wstring& ini) {
    auto profile=ReadProfile(ini);
    struct Scope{const Config::Profile* previous;Scope(const Config::Profile* p):previous(loadingProfile){loadingProfile=p;}~Scope(){loadingProfile=previous;}} scope(profile.get());
    Config c;c.english=Read(ini,L"Language",L"English",L"0")==L"1";toolEnglish.store(c.english);c.profile=profile;c.ini=ini;c.server=Read(ini,L"License",L"Server",L"http://127.0.0.1:5187");c.key=Read(ini,L"License",L"Key",L"");
    const auto mode=Read(ini,L"Runtime",L"Mode",L"local");
    c.mode=mode==L"local"?RunMode::Local:mode==L"server"?RunMode::Server:RunMode::Disabled;
    if(Read(ini,L"Runtime",L"Enabled",L"1")!=L"1")c.mode=RunMode::Disabled;
    c.camera=Number(Read(ini,L"Event",L"Camera",L"2800"),2800,500,10000);
    c.fogZ=Number(Read(ini,L"Event",L"FogZ",L"20000"),20000,1,50000);
    c.startDelay=Number(Read(ini,L"Event",L"Delay Start Game",L"6"),6,0,60);
    ParseFpsLimit(Read(ini,L"Graphics",L"Fps Limit",L"0"),c.fpsLimit);
    ParseSceneLight(Read(ini,L"Event",L"Light",L"0"),c.sceneLight);
    c.showFps=Read(ini,L"Event",L"Show FPS",L"0")==L"1";
    c.showResource=Read(ini,L"Event",L"Show Resource",L"0")==L"1";
    c.showHotkeyInPanel=Read(ini,L"Event",L"Show Hotkey In Panel",L"0")==L"1";
    c.showCooldown=Read(ini,L"Event",L"ShowCooldown",L"1")==L"1";
    c.showTimer=Read(ini,L"Event",L"Show Timer",L"1")==L"1";
    c.showMem=Read(ini,L"Event",L"Show Mem",L"0")==L"1"||
              Read(ini,L"Event",L"Show Memory",L"0")==L"1"||
              Read(ini,L"Graphics",L"Show Mem",L"0")==L"1"||
              Read(ini,L"Graphics",L"Show Memory",L"0")==L"1";
    c.showRegenHP=Read(ini,L"Event",L"ShowRegenHP",L"1")==L"1";
    c.showRegenMana=Read(ini,L"Event",L"ShowRegenMana",L"1")==L"1";
    c.soundStartGame=Read(ini,L"Event",L"Sound Start Game",L"0")==L"1";
    c.removeFogEffect=Read(ini,L"Event",L"Remove Fog Effect",L"0")==L"1";
    c.fogClick=Read(ini,L"Event",L"Fog Click",L"0")==L"1";
    const auto revealSetting=Read(ini,L"Event",L"Reveal Invisibles",L"");
    c.revealInvisible=revealSetting.empty()?
                      (Read(ini,L"Event",L"RevealInvisible",L"0")==L"1"||
                       Read(ini,L"Graphics",L"RevealInvisible",L"0")==L"1"||
                       Read(ini,L"hmaph",L"maph_5",L"0")==L"1"):revealSetting==L"1";
    c.showNotiKickBan=Read(ini,L"Event",L"Show Noti Kick/Ban",L"1")==L"1";
    c.notiLogSaveGame=Read(ini,L"Event",L"Noti/Log Info Players Save Game",L"0")==L"1";
    c.modeCast=Read(ini,L"Event",L"ModeCast",L"0")==L"1"?1:0;
    c.lifeManaCommas=Read(ini,L"Graphics",L"Text Life/Mana Commas",L"0")==L"1";
    c.lifeManaNoHide=Read(ini,L"Graphics",L"Text Life/Mana No Hide",L"0")==L"1";
    c.statsTooltip=Read(ini,L"Graphics",L"Detailed Stats Tooltip",L"1")==L"1";
    c.statsCommas=Read(ini,L"Graphics",L"Text Stats Commas",L"0")==L"1";
    c.vertexColor=Read(ini,L"Graphics",L"VertexColor",L"0")==L"1";
    c.colorPlayer=Read(ini,L"Graphics",L"Color Player",L"0")==L"1";
    c.hideInvulnerableBars=Read(ini,L"Graphics",L"BarInvul",L"0")==L"1";
    // Mana bars were removed from the 1.31 port at the user's request.
    c.manaBarEnable=c.manaBarHero=c.manaBarNonHero=false;
    // Group 1: HPBar, StateTag & Dimensions
    c.hpBarColor=ParseHexColor(Read(ini,L"HPBar",L"Color",L"0xFF32CD32"),0xFF32CD32);
    c.hpBarColorEnemy=ParseHexColor(Read(ini,L"HPBar",L"Color Enemy",L"0xFFff4242"),0xFFFF4242);
    c.stateTagEnable=Read(ini,L"StateTag",L"Enable",L"0")==L"1";
    c.stateTagHero=Read(ini,L"StateTag",L"EnableForHero",L"1")==L"1";
    c.stateTagNonHero=Read(ini,L"StateTag",L"EnableForNonHero",L"1")==L"1";
    c.stateTagShowMax=Read(ini,L"StateTag",L"ShowMaxValues",L"0")==L"1";
    c.stateTagHpValue=Read(ini,L"StateTag",L"ShowHpValue",L"1")==L"1";
    c.stateTagManaValue=Read(ini,L"StateTag",L"ShowManaValue",L"1")==L"1";
    c.stateTagFontSize=Number(Read(ini,L"StateTag",L"FontSize",L"4.0"),4.0f,1.0f,20.0f);
    c.stateTagShowLevel=Read(ini,L"StateTag",L"ShowLevelHero",L"1")==L"1";
    c.barWidthHp=Number(Read(ini,L"Event",L"whp",L"0"),0.0f,0.0f,100.0f);
    c.barWidthMp=Number(Read(ini,L"Event",L"wmp",L"0"),0.0f,0.0f,100.0f);
    c.barHeightHp=Number(Read(ini,L"Event",L"hhp",L"0"),0.0f,0.0f,100.0f);
    c.barHeightMp=Number(Read(ini,L"Event",L"hmp",L"0"),0.0f,0.0f,100.0f);
    c.localDelay=static_cast<int>(Number(Read(ini,L"Event",L"Local Game Delay Reducer",L"100"),100,1,1000));
    c.lanDelay=static_cast<int>(Number(Read(ini,L"Event",L"Lan Game Delay Reducer",L"100"),100,1,1000));
    c.netDelay=static_cast<int>(Number(Read(ini,L"Event",L"Net Game Delay Reducer",L"250"),250,1,1000));
    c.wide=Read(ini,L"Event",L"WideScreen",L"0")==L"1";
    c.borderless=Read(ini,L"Window",L"Borderless",L"0")==L"1";
    c.lockCursor=Read(ini,L"Window",L"Lock Cursor",L"0")==L"1";
    c.unlockMap=Read(ini,L"Event",L"Unlock Map Size",L"0")==L"1";
    c.quickDropItem=Read(ini,L"Event",L"Quick Drop Item",L"0")==L"1";
    c.autoPickItem=Read(ini,L"Event",L"Auto Pick Item",L"0")==L"1";
    c.showAutoPick=Read(ini,L"Event",L"Show Auto Pick",L"1")==L"1";
    c.showDropAll=Read(ini,L"Event",L"Show Drop All",L"1")==L"1";
    c.showSellAll=Read(ini,L"Event",L"Show Sell All",L"1")==L"1";
    c.showInsAllSkills=Read(ini,L"Event",L"Show Ins All Skills",L"1")==L"1";
    c.showUnitSkills=Read(ini,L"Event",L"Show Unit Skills",L"1")==L"1";
    c.unlockFps=Read(ini,L"Graphics",L"Unlock Limit FPS",L"0")==L"1";
    c.hideTimeOfDay=Read(ini,L"Graphics",L"timeOfDayUI",L"0")==L"1";
    c.hideBlackBar=Read(ini,L"Graphics",L"BlackBarUI",L"0")==L"1";
    c.hideConsoleUI=Read(ini,L"Graphics",L"ConsoleUI",L"0")==L"1";
    c.mouseTooltip=Read(ini,L"Graphics",L"MouseTooltipUI",L"0")==L"1";
    c.fastMenu=Read(ini,L"Event",L"Fast Menu",L"0")==L"1";
    c.hideOptionMenu=Read(ini,L"Event",L"Hide Option Menu",L"0")==L"1";
    c.clearTextEnabled=Read(ini,L"DelayClear",L"Enable Delay Clear Text Screen",L"0")==L"1";
    c.clearChatEnabled=Read(ini,L"DelayClear",L"Enable Delay Clear Chat Screen",L"0")==L"1";
    // ETS's 17-entry dropdown stores seconds 1..17, not its zero-based UI index.
    const auto clearSeconds=[&](const wchar_t* key){
        float value=Number(Read(ini,L"DelayClear",key,L"3"),3,1,17);
        return value==std::floor(value)?static_cast<unsigned>(value):3u;
    };
    c.clearTextSeconds=clearSeconds(L"Delay Clear Text Screen");
    c.clearChatSeconds=clearSeconds(L"Delay Clear Chat Screen");
    c.keyboardScroll=static_cast<int>(Number(Read(ini,L"Event",L"Keyboard Scroll Speed",L"100"),100,0,700));
    c.mouseScroll=static_cast<int>(Number(Read(ini,L"Event",L"Mouse Scroll Speed",L"100"),100,0,700));
    c.inventoryEnabled=Read(ini,L"SlotHotkeyInv",L"Enable",L"0")==L"1";
    c.commandEnabled=Read(ini,L"SlotHotkey",L"Enable",L"0")==L"1";
    c.shopEnabled=Read(ini,L"SlotHotkeyShop",L"Enable",L"0")==L"1";
    LoadBindings(ini,L"SlotHotkeyInv",L"SlotInventory",L"QuickCastInventory",c.inventory);
    LoadBindings(ini,L"SlotHotkey",L"SlotCommand",L"QuickCastCommand",c.command);
    LoadBindings(ini,L"SlotHotkeyShop",L"SlotCommandShop",L"QuickCastCommandShop",c.shop);
    for(size_t i=0;i<c.heroes.size();++i)c.heroes[i]=ReadKey(ini,L"HotKeyHero",(L"HeroF"+std::to_wstring(i+1)).c_str());
    c.autoClickLeftKey=ReadKey(ini,L"HotKeyHero",L"AutoClickLeft",L"-1");
    c.autoClickRightKey=ReadKey(ini,L"HotKeyHero",L"AutoClickRight",L"-1");
    c.autoClickInterval=static_cast<unsigned>(Number(Read(ini,L"HotKeyHero",L"AutoClickInterval",L"10"),10,5,1000));
    c.setIdTargetKey=ReadKey(ini,L"HotKeyHero",L"SetIdTarget",L"-1");
    c.sellFastKey=ReadKey(ini,L"HotKeyHero",L"SellFast",L"-1");
    c.macroEnabled=Read(ini,L"MacroRecorder",L"Enabled",L"1")==L"1";c.macroShowButton=Read(ini,L"MacroRecorder",L"ShowButton",L"1")==L"1";
    {const wchar_t* keys[]={L"PlayKey",L"StopKey",L"RecordKey",L"ScriptKey"};for(size_t i=0;i<4;++i)c.macroKeys[i]=ReadKey(ini,L"MacroRecorder",keys[i],L"-1");}
    c.autoManaEnabled=Read(ini,L"AutoMana",L"Enabled",L"0")==L"1";
    c.autoManaPercent=Number(Read(ini,L"AutoMana",L"Percent",L"35"),35,1,99);
    c.autoManaItems=HealItemCodes(Read(ini,L"AutoMana",L"ItemTypes",L""));
    c.autoHealEnabled=Read(ini,L"AutoHeal",L"Enabled",L"0")==L"1";
    c.autoHealPercent=Number(Read(ini,L"AutoHeal",L"Percent",L"35"),35,1,99);
    c.autoHealItems=HealItemCodes(Read(ini,L"AutoHeal",L"ItemTypes",L""));
    c.autoSkillsEnabled=Read(ini,L"AutoSkills",L"AutoSkills_Enable",L"0")==L"1";
    c.autoSkillsDelay=Number(Read(ini,L"AutoSkills",L"AutoSkills_Delay",L"0.5"),.5f,.05f,10);
    c.autoSkillsRange=Number(Read(ini,L"AutoSkills",L"AutoSkills_Range",L"1024"),1024,128,4096);
    c.autoSkillsKey=ReadKey(ini,L"AutoSkills",L"AutoSkills_Active",L"-1");
    c.autoCourierEnable=Read(ini,L"Courier",L"AutoCourier",L"1")==L"1";
    c.courierHotkey=ReadKey(ini,L"Courier",L"CourierHotkey",L"-1");
    c.courierBackHotkey=ReadKey(ini,L"Courier",L"BackHotkey",L"-1");
    c.chayGa1Hotkey=ReadKey(ini,L"MTHKEY",L"ChayGa1",L"-1");
    c.chayGa2Hotkey=ReadKey(ini,L"MTHKEY",L"ChayGa2",L"-1");
    c.chayGa3Hotkey=ReadKey(ini,L"MTHKEY",L"ChayGa3",L"-1");
    c.chaySoi1Hotkey=ReadKey(ini,L"MTHKEY",L"ChaySoi1",L"-1");
    c.chaySoi2Hotkey=ReadKey(ini,L"MTHKEY",L"ChaySoi2",L"-1");
    c.chaySoi3Hotkey=ReadKey(ini,L"MTHKEY",L"ChaySoi3",L"-1");
    c.chayGa1Count=static_cast<int>(Number(Read(ini,L"MTHKEY",L"ChayGa1_count",L"5"),5,1,20));
    c.chayGa2Count=static_cast<int>(Number(Read(ini,L"MTHKEY",L"ChayGa2_count",L"5"),5,1,20));
    c.chayGa3Count=static_cast<int>(Number(Read(ini,L"MTHKEY",L"ChayGa3_count",L"5"),5,1,20));
    c.chaySoi1Count=static_cast<int>(Number(Read(ini,L"MTHKEY",L"ChaySoi1_count",L"5"),5,1,20));
    c.chaySoi2Count=static_cast<int>(Number(Read(ini,L"MTHKEY",L"ChaySoi2_count",L"5"),5,1,20));
    c.chaySoi3Count=static_cast<int>(Number(Read(ini,L"MTHKEY",L"ChaySoi3_count",L"5"),5,1,20));
    c.mth49Enable=Read(ini,L"MTHKEY",L"MTH49Enable",L"1")==L"1";
    c.timeBoss=Read(ini,L"MTHKEY",L"TimeBoss",L"0")==L"1";
    c.g17Enable=Read(ini,L"MTHKEY",L"G17Enable",L"0")==L"1";
    c.mthNoti=Read(ini,L"MTHKEY",L"Noti",L"1")==L"1";
    c.shieldHotkey=ReadKey(ini,L"MTHKEY",L"Shield",L"-1");
    c.iceHotkey=ReadKey(ini,L"MTHKEY",L"Ice",L"-1");
    c.wildHotkey=ReadKey(ini,L"MTHKEY",L"Wild",L"-1");
    c.frozenHotkey=ReadKey(ini,L"MTHKEY",L"Frozen",L"-1");
    c.phoenixHotkey=ReadKey(ini,L"MTHKEY",L"Phoenix",L"-1");
    c.pickGokuHotkey=ReadKey(ini,L"MTHKEY",L"PickGoku",L"-1");
    c.shieldDraw=Read(ini,L"MTHKEY",L"ShieldDraw",L"1")==L"1";
    c.iceDraw=Read(ini,L"MTHKEY",L"IceDraw",L"1")==L"1";
    c.wildDraw=Read(ini,L"MTHKEY",L"WildDraw",L"1")==L"1";
    c.frozenDraw=Read(ini,L"MTHKEY",L"FrozenDraw",L"1")==L"1";
    c.phoenixDraw=Read(ini,L"MTHKEY",L"PhoenixDraw",L"1")==L"1";
    c.pickGokuDraw=Read(ini,L"MTHKEY",L"PickGokuDraw",L"0")==L"1";
    c.autoGaDraw1=Read(ini,L"MTHKEY",L"AutoGaDraw1",L"0")==L"1";
    c.autoGaDraw2=Read(ini,L"MTHKEY",L"AutoGaDraw2",L"0")==L"1";
    c.autoGaDraw3=Read(ini,L"MTHKEY",L"AutoGaDraw3",L"0")==L"1";
    c.autoSoiDraw1=Read(ini,L"MTHKEY",L"AutoSoiDraw1",L"0")==L"1";
    c.autoSoiDraw2=Read(ini,L"MTHKEY",L"AutoSoiDraw2",L"0")==L"1";
    c.autoSoiDraw3=Read(ini,L"MTHKEY",L"AutoSoiDraw3",L"0")==L"1";
    c.allowLocalFiles=Read(ini,L"Event",L"Allow Local Files",L"0")==L"1";
    c.fastJass=Read(ini,L"Event",L"Fast Jass",L"1")==L"1";
    c.oldNames=Read(ini,L"Event",L"OLDNames",L"");
    c.newNames=Read(ini,L"Event",L"NewNames",L"");
    c.oldNamesEx=Read(ini,L"Event",L"OLDNames_Ex",L"");
    c.newNamesEx=Read(ini,L"Event",L"NewNames_Ex",L"");
    c.nameCreate=Read(ini,L"Event",L"NameCreate",L"");
    c.loadDll=Read(ini,L"Event",L"LoadDll",L"");
    c.loadMpq=Read(ini,L"Event",L"LoadMPQ",L"");
    c.codeNen=Read(ini,L"Event",L"code_nen",L"");
    c.war3mapEnable=Read(ini,L"war3map",L"enable",L"0")==L"1";
    c.war3mapLoad=Read(ini,L"war3map",L"load",L"0")==L"1";
    c.war3mapFile=Read(ini,L"war3map",L"file",L"");
    if(c.war3mapFile.empty()) c.war3mapFile=Read(ini,L"war3map",L"script",L"");
    c.refreshRate=static_cast<unsigned>(Number(Read(ini,L"Graphics",L"Refresh Rate",L"0"),0,0,500));
    c.vsync=Read(ini,L"Graphics",L"Vsync",L"0")==L"1";
    c.multiboardKey=ReadKey(ini,L"HotKeyOther",L"hko_MinimizeMultiboard",L"515");
    c.menuKey=ReadKey(ini,L"HotKeyOther",L"hko_menu");
    c.commandKey=ReadKey(ini,L"HotKeyOther",L"hko_command",L"774");
    if(c.menuKey==VK_ESCAPE)c.menuKey=0;
    c.quickChatEnabled=Read(ini,L"Quick Chat",L"Enable",L"0")==L"1";
    for(size_t i=0;i<c.quickChat.size();++i){
        const auto suffix=std::to_wstring(i+1);
        c.quickChat[i].vk=ReadKey(ini,L"Quick Chat",(L"QuickChatHotkey"+suffix).c_str());
        if(c.quickChat[i].vk==VK_ESCAPE)c.quickChat[i].vk=0; // Original ETS excludes Escape.
        c.quickChat[i].text=QuickChatText(Read(ini,L"Quick Chat",(L"QuickChatText"+suffix).c_str(),L""));
    }
    return c;
}
bool SaveSetting(const std::wstring& ini,const wchar_t* section,const wchar_t* key,const std::wstring& value) {
    return WritePrivateProfileStringW(section,key,value.c_str(),ini.c_str())!=FALSE;
}
bool MenuHotkeyMatches(unsigned vk,unsigned binding,bool allowed,bool chat,bool foreground,bool modifiers){
    return binding!=0&&binding!=VK_ESCAPE&&vk==binding&&allowed&&!chat&&foreground&&!modifiers;
}
void Log(const std::wstring& directory,const std::string& text) {
    static std::mutex m;std::lock_guard<std::mutex> lock(m);
    std::ofstream out(std::filesystem::path(directory)/L"WHT.log",std::ios::app);
    SYSTEMTIME t{};GetLocalTime(&t);
    out<<t.wYear<<'-'<<t.wMonth<<'-'<<t.wDay<<' '<<t.wHour<<':'<<t.wMinute<<':'<<t.wSecond<<" "<<text<<"\n";
}
bool ParseLease(const std::string& response,Lease& lease) {
    lease={};if(response.size()>4096)return false;
    std::istringstream input(response);std::string line;bool ok=false,seconds=false,features=false;
    while(std::getline(input,line)) {
        if(!line.empty()&&line.back()=='\r')line.pop_back();
        auto p=line.find('=');if(p==std::string::npos)continue;
        auto key=line.substr(0,p),value=line.substr(p+1);
        if(key=="status"){if(ok||value!="ok")return false;ok=true;}
        if(key=="lease_seconds"){
            if(seconds||value.empty()||value.find_first_not_of("0123456789")!=std::string::npos)return false;
            try {lease.seconds=static_cast<unsigned>(std::stoul(value));}catch(...){return false;}
            if(lease.seconds<1||lease.seconds>120)return false;seconds=true;
        }
        if(key=="features"){
            if(features)return false;features=true;std::istringstream names(value);std::string name;
            while(std::getline(names,name,',')) {
                if(name=="camera")lease.features|=Camera;else if(name=="graphics")lease.features|=Graphics;
                else if(name=="hotkeys")lease.features|=Hotkeys;else if(name=="chat")lease.features|=Chat;
                else if(name=="plugins")lease.features|=Plugins;else if(name=="mth")lease.features|=Mth;
                else if(name=="maptools")lease.features|=MapTools;else return false;
            }
        }
    }
    return ok&&seconds&&features&&lease.features!=0;
}
}
