#pragma once
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <map>
#include <memory>

namespace wht {
// Fold command names only; chat text, paths and four-character IDs retain case.
inline void FoldCommandName(std::wstring& name){for(auto& ch:name)if(ch>=L'A'&&ch<=L'Z')ch+=L'a'-L'A';}

enum Feature : unsigned { Camera=1, Graphics=2, Hotkeys=4, Chat=8, Plugins=16, Mth=32, MapTools=64 };
enum class RunMode { Disabled, Local, Server };
struct Binding { unsigned vk=0; bool enabled=false, quickCast=false, autoTarget=false, autoTargetMe=false; };
struct ChatBinding { unsigned vk=0; std::string text; };
inline std::atomic<bool> toolEnglish{false};
struct Config {
    bool english=false;
    using Profile = std::map<std::wstring,std::map<std::wstring,std::wstring>>;
    std::shared_ptr<const Profile> profile;
    std::wstring ini, server, key;
    RunMode mode=RunMode::Local;
    float camera=2800, fogZ=20000, startDelay=6;
    int localDelay=100, lanDelay=100, netDelay=250;
    int keyboardScroll=100, mouseScroll=100;
    unsigned fpsLimit=0; // 0 leaves the engine setting alone; otherwise 60..1000.
    unsigned sceneLight=0; // 0 restores engine ownership; 1..255 ambient grayscale.
    bool showFps=false;
    bool showPing=false;
    bool showResource=false;
    bool showHotkeyInPanel=false,showCooldown=true;
    bool showTimer=true,showRegenHP=true,showRegenMana=true;
    bool showMem=false;
    bool soundStartGame=false;
    bool removeFogEffect=false;
    bool fogClick=false;
    bool revealInvisible=true;
    bool showNotiKickBan=true;
    bool notiLogSaveGame=false;
    int modeCast=0; // 0 = Normal, 1 = Quick Cast (reference 1.29)
    bool lifeManaCommas=false,lifeManaNoHide=false;
    bool statsCommas=false;
    bool statsTooltip=true;
    bool vertexColor=false;
    bool colorPlayer=false;
    bool hideInvulnerableBars=false;
    bool inventoryEnabled=false, commandEnabled=false, shopEnabled=false;
    std::array<Binding,6> inventory{};
    std::array<Binding,12> command{};
    std::array<Binding,12> shop{};
    std::array<unsigned,7> heroes{};
    unsigned multiboardKey=VK_TAB;
    unsigned menuKey=0,commandKey=VK_F7;
    bool quickChatEnabled=false;
    std::array<ChatBinding,9> quickChat{};
    bool hideTimeOfDay=false;
    bool hideBlackBar=false;
    bool hideConsoleUI=false;
    bool mouseTooltip=false;
    bool fastMenu=false;
    bool hideOptionMenu=false;
    bool clearTextEnabled=false,clearChatEnabled=false;
    unsigned clearTextSeconds=3,clearChatSeconds=3;
    bool wide=false, borderless=false, lockCursor=false, unlockMap=false, unlockFps=false;
    // Group 1: HPBar, ManaBar, StateTag & Dimensions
    uint32_t hpBarColor=0xFF32CD32;
    uint32_t hpBarColorEnemy=0xFFFF4242;
    bool manaBarEnable=false;
    uint32_t manaBarColor=0xFF08FCDC;
    uint32_t manaBarColorEnemy=0xFF47B9FF;
    bool manaBarHero=false;
    bool manaBarNonHero=false;
    bool stateTagEnable=false;
    bool stateTagHero=true;
    bool stateTagNonHero=true;
    bool stateTagShowMax=false;
    bool stateTagHpValue=true, stateTagManaValue=true;
    float stateTagFontSize=4.0f;
    bool stateTagShowLevel=true;
    float barWidthHp=0.0f;
    float barWidthMp=0.0f;
    float barHeightHp=0.0f;
    float barHeightMp=0.0f;
    bool quickDropItem=false;
    bool autoPickItem=false;
    bool showAutoPick=true;
    bool showDropAll=true;
    bool showSellAll=true;
    bool showInsAllSkills=true;
    bool showUnitSkills=true;
    unsigned autoClickLeftKey=0;
    unsigned autoClickRightKey=0;
    unsigned autoClickInterval=10;
    unsigned setIdTargetKey=0,sellFastKey=0;
    bool macroEnabled=true,macroShowButton=true;std::array<unsigned,4> macroKeys{};
    bool autoManaEnabled=false;float autoManaPercent=35;std::vector<uint32_t> autoManaItems;
    bool autoHealEnabled=false;float autoHealPercent=35;std::vector<uint32_t> autoHealItems;
    bool autoSkillsEnabled=false;
    float autoSkillsDelay=.5f,autoSkillsRange=1024;
    unsigned autoSkillsKey=0;
    bool autoCourierEnable=true;
    unsigned courierHotkey=0;
    unsigned courierBackHotkey=0;
    unsigned chayGa1Hotkey=0;
    unsigned chayGa2Hotkey=0;
    unsigned chayGa3Hotkey=0;
    unsigned chaySoi1Hotkey=0;
    unsigned chaySoi2Hotkey=0;
    unsigned chaySoi3Hotkey=0;
    int chayGa1Count=5;
    int chayGa2Count=5;
    int chayGa3Count=5;
    int chaySoi1Count=5;
    int chaySoi2Count=5;
    int chaySoi3Count=5;
    bool mth49Enable=true;
    bool timeBoss=false;
    bool g17Enable=false;
    bool mthNoti=true;
    unsigned shieldHotkey=0;
    unsigned iceHotkey=0;
    unsigned wildHotkey=0;
    unsigned frozenHotkey=0;
    unsigned phoenixHotkey=0;
    unsigned pickGokuHotkey=0;
    bool shieldDraw=true;
    bool iceDraw=true;
    bool wildDraw=true;
    bool frozenDraw=true;
    bool phoenixDraw=true;
    bool pickGokuDraw=false;
    bool autoGaDraw1=false;
    bool autoGaDraw2=false;
    bool autoGaDraw3=false;
    bool autoSoiDraw1=false;
    bool autoSoiDraw2=false;
    bool autoSoiDraw3=false;
    bool allowLocalFiles=false;
    bool fastJass=false;
    bool war3mapEnable=false;
    bool war3mapLoad=false;
    std::wstring war3mapFile;
    unsigned refreshRate=0;
    bool vsync=false;
    std::wstring oldNames;
    std::wstring newNames;
    std::wstring oldNamesEx;
    std::wstring newNamesEx;
    std::wstring nameCreate;
    std::wstring loadDll;
    std::wstring loadMpq;
    std::wstring codeNen;
};
struct Lease { unsigned features=0; unsigned seconds=0; };

enum class War3Version : uint32_t {
    Unknown = 0,
    v124e = 6387,
    v126a = 6401
};

struct GameOffsets {
    War3Version version = War3Version::v124e;
    uintptr_t root = 0xACBDD8;
    uintptr_t world = 0xACD44C;
    uintptr_t overheadBarMgr = 0xACD0F0;
    uintptr_t matchTimer = 0xACE1C0;
    uintptr_t tlsIndex = 0xACEA4C;
    uintptr_t netPlayer = 0xAE3CDC;
    uintptr_t recipeRoot = 0xAE54CC;
    uintptr_t menuRoot = 0xAE3A9C;
    uintptr_t chatOpen = 0xAE8450;
    uintptr_t localMessage = 0x2F9980;

    // Functions
    uintptr_t getUnitManager = 0x3A8BA0;
    uintptr_t lookupUnit = 0x4317C0;
    uintptr_t war3MapScriptLoader = 0x3B2560;
    uintptr_t unitVisibilityCheck = 0x39A440;

    // Hero API
    uintptr_t heroSelect = 0x3C8450;
    uintptr_t heroX = 0x3C6050;
    uintptr_t heroY = 0x3C6090;
    uintptr_t heroCenter = 0x3B5110;
    uintptr_t heroClear = 0x3BC5E0;
    uintptr_t heroIsUnitType = 0x3C89D0;
    uintptr_t getHeroLevel = 0x3C7A10;

    // Life/Mana fonts
    uintptr_t fontEntry = 0x60D1B0;
    uintptr_t fontWidth = 0x60D750;

    // Hooks
    uintptr_t cameraHook = 0x3065A0;
    uintptr_t cameraCallsite = 0x3B5420;
    uintptr_t keyScrollHook = 0x303530;
    uintptr_t mouseScrollHook = 0x3034F0;
    uintptr_t perspectiveHook = 0x7B6E90;
    uintptr_t menuTransitionHook = 0x594130;
    uintptr_t gameEventHook = 0x5383E0;
    uintptr_t eventContext = 0x4C3FD0;
    uintptr_t fogEffectHook = 0x38C940;
    uintptr_t lifeManaFontHook = 0x60D1B0;
    uintptr_t sceneLightHook = 0x52FD70;
    uintptr_t vertexColorHook = 0x29E3A0;
    uintptr_t barUpdateHook = 0x2C7FD0;
    uintptr_t chatChannelSend = 0x5AFE70;
    uintptr_t selectionPacket = 0x2CF790;
    uintptr_t saveGameHandler = 0x43FA80;
    uintptr_t skillCard=0x34FDC0,skillCardExtra=0x34FE00;
    uintptr_t skillEnumerate=0x277FE0,skillEnumerateExtra=0x278090;
    uintptr_t skillGate=0x28ECF0,skillUnitVtable=0x943A94;

    // Cooldown overlay vtables
    uintptr_t vtCamera1 = 0x88A60C;
    uintptr_t vtCamera2 = 0x88A638;
    uintptr_t fnExpected1 = 0x478500;
    uintptr_t fnExpected2 = 0x478690;
    uintptr_t vtStock = 0x888EA0;
    uintptr_t vtAbility1 = 0x88F4D4;
    uintptr_t vtAbility2 = 0x88FB74;

    // Native Bar UI API
    uintptr_t setTextureColor = 0x60EEE0;
    uintptr_t setWidth = 0x606530;
    uintptr_t setHeight = 0x606550;
    uintptr_t setRelPoint = 0x606F10;
    uintptr_t setAbsPoint = 0x606950;
    uintptr_t frameCtor = 0x60A410;
    uintptr_t backdropCtor = 0x61DD40;
    uintptr_t setBackdrop = 0x608CF0;
    uintptr_t stringAssign = 0x4C67F0;
    uintptr_t fontCtor = 0x60DD20;
    uintptr_t setFont = 0x60E1C0;
    uintptr_t setTextAlign = 0x60E290;
    uintptr_t setText = 0x60D1B0;

    // Mana Bar
    uintptr_t manaBarCtor = 0x35A800;
    uintptr_t manaBarVtable = 0x95075C;
    uintptr_t manaBarBind = 0x365590;
    uintptr_t unitState = 0x3C5F40;
    uintptr_t unitType = 0x3C89D0;
    uintptr_t localPlayer = 0x3BC6A0;
    uintptr_t unitEnemy = 0x3C8610;

    // Resource UI
    uintptr_t resGetter1 = 0x3CA140;
    uintptr_t resGetter2 = 0x3C1E10;
    uintptr_t resGetter3 = 0x3CA640;
    uintptr_t resGetter4 = 0x3C1D80;
    uintptr_t resFont = 0x60DD20;
    uintptr_t resVtable1Ptr = 0x97FC64;
    uintptr_t resVtable1Val = 0x60A7A0;
    uintptr_t resVtable2Ptr = 0x9800B4;
    uintptr_t resVtable2Val = 0x60E760;
    uintptr_t resNameGetter = 0x40BB30;

    // Resource Transfer
    uintptr_t transferPlayer = 0x3BC670;
    uintptr_t transferLocal = 0x3BC6A0;
    uintptr_t transferAlly = 0x3CA070;
    uintptr_t transferSend = 0x54E470;
    uintptr_t transferVtablePtr = 0x944EB0;
    uintptr_t transferVtableVal = 0x4C26B0;

    // Bar & Vertex Color
    uintptr_t barInvulTarget = 0x37C5F0;
    uintptr_t barRefresh = 0x333240;
    uintptr_t vertexVtablePtr = 0x943B48;

    // Font callsites
    uintptr_t fontCalls[4] = { 0x358CFD, 0x358D25, 0x358F27, 0x358F4F };
    uintptr_t statsWrapper = 0x3471A0;
    uintptr_t statsCalls[6] = { 0x3471FF, 0x354572, 0x354B35, 0x355525, 0x3555D9, 0x355656 };

    // General game functionality
    uintptr_t btnVtable = 0x950D1C;
    uintptr_t cmdManager = 0xAE55B8;
    uintptr_t lockTable = 0xAE58C8;
    uintptr_t enterLock = 0x6D9FC0;
    uintptr_t leaveLock = 0x6D90F0;
    uintptr_t mouseDispatch = 0x632C00;
    uintptr_t targetOrder = 0x339D50;
    uintptr_t buttonClick = 0x603BE0;
    uintptr_t executeJass = 0x3D4A70;
    uintptr_t multiboardMinimize = 0x38A260;
    uintptr_t multiboardMaximize = 0x38A340;
    uintptr_t fpsDisplay = 0x33C590;
    uintptr_t setDefaultPoints = 0x606F90;
    uintptr_t timeOfDayShow = 0x5FEE90;
    uintptr_t timeOfDayHide = 0x5FEE30;
    uintptr_t chatOpenFn = 0x341FA0;
    uintptr_t chatTextFn = 0x341F50;
    uintptr_t chatSubmitFn = 0x2FD240;
    uintptr_t chatSubmitArg = 0xAB0CD0;
    uintptr_t resPlayerController = 0x3C1DF0;
    uintptr_t resPlayerNameBySlot = 0x2F9AD0;
    uintptr_t resTextVtable = 0x9800AC;
    uintptr_t lobbySlot = 0x53F8D0;
    uintptr_t lobbyName = 0x53F900;
    uintptr_t lobbySlotUpdate = 0x561980;
    uintptr_t lobbyTextSetter = 0x6124E0;
    uintptr_t lobbyHostCheck = 0x53FC10;
    uintptr_t lobbyPlayerId = 0x54FEF0;
    uintptr_t setTerrainType = 0x3BDA60;
    uintptr_t textureCtor = 0x60E0A0;
    uintptr_t setTexture = 0x60E830;
    uintptr_t setVisible = 0x606460;
    uintptr_t resolveUnitFromTags = 0x40770;
    uintptr_t buttonCtor = 0x603880;
    uintptr_t setButtonTexture = 0x6039C0;
    uintptr_t setButtonState = 0x603780;
    uintptr_t actionPacketVtable = 0x944E8C;
    uintptr_t actionPacketPlayer = 0x3BF690;
    uintptr_t fpsBarVtable = 0x94F38C;
    uintptr_t tooltipVtable = 0x9517E4;
    uintptr_t renderCounter = 0xAE2B98;
    uintptr_t startDelayRva = 0x971D0C;
    uintptr_t localDelayRva = 0x6616A1;
    uintptr_t lanDelayRva = 0x662231;
    uintptr_t netDelayRva = 0x65E5E1;
    uintptr_t size1Rva = 0x657F84;
    uintptr_t size2Rva = 0x66F51F;
    uintptr_t size3Rva = 0x67F401;
    uintptr_t unlockFpsRva = 0x62DF9B;
    uintptr_t showResourcesRva = 0x36058A;
    uintptr_t fpsGet = 0x62B840;
    uintptr_t fpsSet = 0x62B7E0;
    uintptr_t clearText = 0x3B59A0;
    uintptr_t clearChat = 0x604D80;
    uintptr_t chatDisplay = 0x2FBFC0;
    uintptr_t handleToUnit = 0x3BE7F0;
    uintptr_t issueImmediateOrder = 0x33A7A0;
    uintptr_t issuePointOrder = 0x33A910;
    uintptr_t unitGetAbility = 0x79400;
    uintptr_t cursorManager = 0x39C220;
};

inline GameOffsets GetGameOffsets(War3Version version) {
    GameOffsets o;
    if (version == War3Version::v126a) {
        o.version = War3Version::v126a;
        o.root = 0xAB4F80;
        o.world = 0xAB65F4;
        o.overheadBarMgr = 0xAB6298;
        o.matchTimer = 0xAB7368;
        o.tlsIndex = 0xAB7BF4;
        o.netPlayer = 0xACCE80;
        o.recipeRoot = 0xACE66C;
        o.menuRoot = 0xACCC44;
        o.chatOpen = 0xAD15F0;
        o.localMessage = 0x2F8E40;
        o.chatDisplay = 0x2FB480;

        o.getUnitManager = 0x3A8060;
        o.lookupUnit = 0x430C80;
        o.war3MapScriptLoader = 0x3B1A20;
        o.unitVisibilityCheck = 0x399900;

        o.heroSelect = 0x3C7910;
        o.heroX = 0x3C5510;
        o.heroY = 0x3C5550;
        o.heroCenter = 0x3B45D0;
        o.heroClear = 0x3BBAA0;
        o.heroIsUnitType = 0x3C7E90;

        o.fontEntry = 0x60CA10;
        o.fontWidth = 0x60CFB0;

        o.cameraHook = 0x305A60;
        o.cameraCallsite = 0x3B48E0;
        o.keyScrollHook = 0x3029F0;
        o.mouseScrollHook = 0x3029B0;
        o.perspectiveHook = 0x7B66F0;
        o.menuTransitionHook = 0x593990;
        o.gameEventHook = 0x5378E0;
        o.eventContext = 0x4C34D0;
        o.fogEffectHook = 0x38BE00;
        o.lifeManaFontHook = 0x60CA10;
        o.sceneLightHook = 0x52F270;
        o.vertexColorHook = 0x29D880;
        o.barUpdateHook = 0x2C74B0;
        o.chatChannelSend = 0x5AF6D0;
        o.selectionPacket = 0x2CEC70;
        o.saveGameHandler = 0x43EF70;

        o.vtCamera1 = 0x8784AC;
        o.vtCamera2 = 0x8784D8;
        o.fnExpected1 = 0x4779F0;
        o.fnExpected2 = 0x477B80;
        o.vtStock = 0x876D40;
        o.vtAbility1 = 0x87D374;
        o.vtAbility2 = 0x87DA14;

        o.setTextureColor = 0x60E740;
        o.setWidth = 0x605D90;
        o.setHeight = 0x605DB0;
        o.setRelPoint = 0x606770;
        o.setAbsPoint = 0x6061B0;
        o.frameCtor = 0x609C70;
        o.backdropCtor = 0x61D5A0;
        o.setBackdrop = 0x608550;
        o.stringAssign = 0x4C5CF0;
        o.fontCtor = 0x60D580;
        o.setFont = 0x60DA20;
        o.setTextAlign = 0x60DAF0;
        o.setText = 0x60CA10;

        o.manaBarCtor = 0x359CC0;
        o.manaBarVtable = 0x93E604;
        o.manaBarBind = 0x364A50;
        o.unitState = 0x3C5400;
        o.unitType = 0x3C7E90;
        o.getHeroLevel = 0x3C6ED0;
        o.localPlayer = 0x3BBB60;
        o.unitEnemy = 0x3C7AD0;

        o.resGetter1 = 0x3C9600;
        o.resGetter2 = 0x3C12D0;
        o.resGetter3 = 0x3C9B00;
        o.resGetter4 = 0x3C1240;
        o.resFont = 0x60D580;
        o.resVtable1Ptr = 0x96DFBC;
        o.resVtable1Val = 0x60A000;
        o.resVtable2Ptr = 0x96E40C;
        o.resVtable2Val = 0x60DFC0;
        o.resNameGetter = 0x40AFF0;

        o.transferPlayer = 0x3BBB30;
        o.transferLocal = 0x3BBB60;
        o.transferAlly = 0x3C9530;
        o.transferSend = 0x54D970;
        o.transferVtablePtr = 0x932D50;
        o.transferVtableVal = 0x4C1BB0;

        o.barInvulTarget = 0x37BAB0;
        o.barRefresh = 0x332700;
        o.vertexVtablePtr = 0x9319E8;

        o.fontCalls[0] = 0x3581BD;
        o.fontCalls[1] = 0x3581E5;
        o.fontCalls[2] = 0x3583E7;
        o.fontCalls[3] = 0x35840F;
        o.statsWrapper = 0x346660;
        o.statsCalls[0] = 0x3466BF;
        o.statsCalls[1] = 0x353A32;
        o.statsCalls[2] = 0x353FF5;
        o.statsCalls[3] = 0x3549E5;
        o.statsCalls[4] = 0x354A99;
        o.statsCalls[5] = 0x354B16;

        o.btnVtable = 0x93EBC4;
        o.cmdManager = 0xACE758;
        o.lockTable = 0xACEA68;
        o.enterLock = 0x6D9820;
        o.leaveLock = 0x6D8950;
        o.mouseDispatch = 0x632460;
        o.targetOrder = 0x339210;
        o.buttonClick = 0x603440;
        o.executeJass = 0x3D3F30;
        o.multiboardMinimize = 0x389720;
        o.multiboardMaximize = 0x389800;
        o.fpsDisplay = 0x33BA50;
        o.setDefaultPoints = 0x6067F0;
        o.timeOfDayShow = 0x5FE6F0;
        o.timeOfDayHide = 0x5FE690;
        o.chatOpenFn = 0x341460;
        o.chatTextFn = 0x341410;
        o.chatSubmitFn = 0x2FC700;
        o.chatSubmitArg = 0xA9ACB0;
        o.resPlayerController = 0x3C12B0;
        o.resPlayerNameBySlot = 0x2F8F90;
        o.resTextVtable = 0x96E404;
        o.lobbySlot = 0x53EDD0;
        o.lobbyName = 0x53EE00;
        o.lobbySlotUpdate = 0x560E80;
        o.lobbyTextSetter = 0x611D40;
        o.lobbyHostCheck = 0x53F110;
        o.skillCard=0x34F280;o.skillCardExtra=0x34F2C0;
        o.skillEnumerate=0x2774C0;o.skillEnumerateExtra=0x277570;
        o.skillGate=0x28E1D0;o.skillUnitVtable=0x931934;
        o.lobbyPlayerId = 0x54F3F0;
        o.setTerrainType = 0x3BCF20;
        o.textureCtor = 0x60D900;
        o.setTexture = 0x60E090;
        o.setVisible = 0x605CC0;
        o.resolveUnitFromTags = 0x3FA30;
        o.buttonCtor = 0x6030E0;
        o.setButtonTexture = 0x603220;
        o.setButtonState = 0x602FE0;
        o.actionPacketVtable = 0x932D2C;
        o.actionPacketPlayer = 0x3BEB50;
        o.fpsBarVtable = 0x93D22C;
        o.tooltipVtable = 0x93F68C;
        o.renderCounter = 0xACBD40;
        o.startDelayRva = 0x95FBB4;
        o.localDelayRva = 0x660F01;
        o.lanDelayRva = 0x661A91;
        o.netDelayRva = 0x65DE41;
        o.size1Rva = 0x6577E4;
        o.size2Rva = 0x66ED7F;
        o.size3Rva = 0x67EC61;
        o.unlockFpsRva = 0x62D7FB;
        o.showResourcesRva = 0x35FA4A;
        o.fpsGet = 0x62B0A0;
        o.fpsSet = 0x62B040;
        o.clearText = 0x3B4E60;
        o.clearChat = 0x6045E0;
        o.handleToUnit = 0x3BDCB0;
        o.issueImmediateOrder = 0x339C60;
        o.issuePointOrder = 0x339DD0;
        o.unitGetAbility = 0x787D0;
        o.cursorManager = 0x39B6E0;
    }
    return o;
}

inline GameOffsets& ActiveGameOffsets() {
    static GameOffsets s_offsets;
    return s_offsets;
}

inline void* LookupJassNative(uintptr_t game, const char* name) {
    if (!game || !name) return nullptr;
    struct Entry { const char* name; uintptr_t rva; };
    static const Entry table[] = {
        { "GetLocalPlayer",          0x3BC6A0 },
        { "Player",                  0x3BC670 },
        { "EndGame",                 0x3BC6F0 },
        { "GetPlayerState",          0x3CA640 },
        { "SetPlayerState",          0x3CA5E0 },
        { "GetPlayerSlotState",      0x3C1E10 },
        { "GetPlayerController",     0x3C1DF0 },
        { "GetPlayerColor",          0x3C1D80 },
        { "IsPlayerAlly",            0x3CA070 },
        { "CreateUnit",              0x3C5D70 },
        { "CreateItem",              0x3BC4E0 },
        { "RemoveUnit",              0x3C8BA0 },
        { "UnitAddAbility",          0x3C8DE0 },
        { "UnitRemoveAbility",       0x3C8E50 },
        { "SetUnitAbilityLevel",     0x3C7CF0 },
        { "SetUnitScale",            0x3C6E10 },
        { "SetUnitPosition",         0x3C6590 },
        { "SetUnitX",                0x3C64B0 },
        { "SetUnitY",                0x3C6520 },
        { "PanCameraTo",             0x3B5240 },
        { "SetCameraPosition",       0x3B5110 },
        { "SelectUnit",              0x3C8450 },
        { "ClearSelection",          0x3BC5E0 },
        { "SetGameSpeed",            0x3BBA10 },
        { "SetCameraField",          0x3B53F0 },
        { "PingMinimap",             0x3B5190 },
        { "SetPlayerAlliance",       0x3C1B90 },
        { "SaveGame",                0x3BCA20 },
        { "UnitItemInSlot",          0x3C8270 },
        { "GetItemTypeId",           0x3C57A0 },
        { "IsItemPawnable",          0x3C5B50 },
        { "GetUnitTypeId",           0x3C6450 },
        { "IsHeroUnitId",            0x3B39D0 },
        { "SelectHeroSkill",         0x3C7830 },
        { "SetHeroLevel",            0x3C78B0 },
        { "SetHeroStr",              0x3C7170 },
        { "SetHeroAgi",              0x3C71E0 },
        { "SetHeroInt",              0x3C7250 },
        { "CreateGroup",             0x3D3440 },
        { "DestroyGroup",            0x3C4AE0 },
        { "GroupRemoveUnit",         0x3C4B30 },
        { "GroupEnumUnitsOfPlayer",  0x3C4E00 },
        { "FirstOfGroup",            0x3C4FA0 },
        { "GroupEnumUnitsInRange",   0x3C4CE0 },
        { "IsUnitType",              0x3C89D0 },
        { "GetWidgetLife",           0x3C51A0 },
        { "ConvertUnitType",         0x3B3250 },
        { "SetUnitMoveSpeed",        0x3C6780 },
        { "GetWorldBounds",          0x3D18A0 },
        { "CreateFogModifierRect",   0x3D19B0 },
        { "FogModifierStart",        0x3C2700 },
        { "FogModifierStop",         0x3C2720 },
        { "DestroyFogModifier",      0x3C26E0 },
        { "IssuePointOrder",         0x3C98E0 },
        { "IssuePointOrderById",     0x3C9480 },
        { "IssueImmediateOrder",     0x3C98C0 },
        { "IssueImmediateOrderById", 0x3C9440 },
        { "IssueTargetOrderById",    0x3C9510 },
        { "UnitDropItemTarget",      0x3C9AE0 },
        { "UnitDropItemPoint",       0x3C9A70 },
        { "UnitDropItemSlot",        0x3C9B50 },
        { "UnitAddItem",             0x3C7F30 },
        { "UnitRemoveItem",          0x3C81A0 },
        { "UnitRemoveItemFromSlot",  0x3C81E0 },
        { "SetItemPosition",         0x3C57C0 },
        { "SetItemVisible",          0x3C5A60 },
        { "GetUnitX",                0x3C6050 },
        { "GetUnitY",                0x3C6090 },
        { "GetPlayerName",           0x3C1AA0 },
        { "GetPlayerId",             0x3CA180 },
        { "GetOwningPlayer",         0x3C8CD0 },
        { "IsLocationVisibleToPlayer", 0x3CA480 },
        { "GetUnitName",             0x3C6490 },
        { "GetPlayerAlliance",       0x3CA8B0 },
        { "SetTerrainType",          0x3BDA60 },
    };
    for (const auto& item : table) {
        if (strcmp(item.name, name) == 0) {
            const uint32_t shift = (ActiveGameOffsets().version == War3Version::v126a) ? 0xB40 : 0;
            return reinterpret_cast<void*>(game + item.rva - shift);
        }
    }
    return nullptr;
}

std::wstring Directory(HMODULE module);
std::string Utf8(const std::wstring& s);
std::wstring Wide(const std::string& s);
std::string Sha256(const void* bytes, size_t length);
std::string HashFile(const std::wstring& path);
Config LoadConfig(const std::wstring& ini);
bool ParseFpsLimit(const std::wstring& text,unsigned& value);
bool ParseSceneLight(const std::wstring& text,unsigned& value);
uint32_t ParseHexColor(const std::wstring& text, uint32_t defaultValue);
unsigned EngineKeyToVirtualKey(int key);
std::string QuickChatText(const std::wstring& text);
int CommandButtonIndex(int slot);
bool SaveSetting(const std::wstring& ini,const wchar_t* section,const wchar_t* key,const std::wstring& value);
bool ParseLease(const std::string& response,Lease& lease);
bool Activate(const Config& config,Lease& lease,std::wstring& error);
void Log(const std::wstring& directory,const std::string& text);
bool QueueCommand(const std::wstring& command);
std::wstring RuntimeStatus();
void RunPanel(HMODULE module,const std::wstring& directory);
bool PostPanelToggle(HWND gameWindow);
bool PanelHotkeyAllowed();
bool MenuHotkeyMatches(unsigned vk,unsigned binding,bool allowed,bool chat,bool foreground,bool modifiers);
}
