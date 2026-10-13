// Included inside the runtime namespace after plugin/native UI services.
wht::IniWatch129 iniWatch129;
wht::MapReveal129 mapReveal129;
wht::TerrainWork129 terrainWork129;
uintptr_t parityNameWorld129=0;
std::string parityAppliedName129;
std::wstring customModelPath129;
uint64_t parityServicesDue129=0;
uintptr_t parityTerrainWorld129=0;
std::wstring parityTerrainCode129;
std::map<std::wstring,std::filesystem::file_time_type> parityLibraryAttempts129;
void LoadParityLibrary129(const std::filesystem::path& path,bool archive){
 auto key=path.wstring();auto stamp=std::filesystem::last_write_time(path);auto previous=parityLibraryAttempts129.find(key);
 if(previous!=parityLibraryAttempts129.end()&&previous->second==stamp)return;
 parityLibraryAttempts129[key]=stamp;LoadPlugin129(key,archive);
}
void RefreshScriptSettings129(){
 const bool allowed=runtimeConfig.mode==wht::RunMode::Local;
 {std::lock_guard<std::mutex> guard(scriptSettingsMutex129);scriptSettings129={allowed,runtimeConfig.war3mapEnable,runtimeConfig.war3mapLoad,runtimeConfig.war3mapFile,runtimeFolder};}
 {std::lock_guard<std::mutex> guard(nameSettingsMutex129);if(nameSettings129.original.empty())nameSettings129.original=wht::Utf8(wht::RegistryPlayerName129());nameSettings129.allowed=allowed;nameSettings129.oldName=wht::Utf8(runtimeConfig.oldNames);nameSettings129.newName=wht::Utf8(runtimeConfig.newNames);nameSettings129.hostName=wht::Utf8(runtimeConfig.nameCreate);}
 requestedAmbient129=allowed?runtimeConfig.sceneLight:0;
}
void ParityNotice129(const std::string& text){
 if(parityRoomLog129){auto colored=wht::NotificationText(text);if(RoomMessage129(parityRoomLog129,colored.c_str()))return;}
 if(!wht::ShowNotification129(executableBase,text,localMessage129))wht::Log(runtimeFolder,"Command: "+text);
}
void ReloadParity129(){runtimeConfig=wht::LoadConfig(runtimeIni);wht::toolEnglish.store(runtimeConfig.english);toolMenu.nextRefresh=0;ApplyScroll129();RefreshScriptSettings129();parityServicesDue129=0;parityLibraryAttempts129.clear();}
bool SaveParity129(const wchar_t* section,const wchar_t* key,const std::wstring& value){
 if(!wht::SaveSetting(runtimeIni,section,key,value)){ParityNotice129("Could not write WHT.ini");return false;}ReloadParity129();return true;
}
void PollParity129(){
 if(runtimeIni.empty()||!executableBase)return;
 if(iniWatch129.Poll(runtimeIni,GetTickCount64())){ReloadParity129();wht::Log(runtimeFolder,"WHT 1.29: configuration reloaded.");}
 mapReveal129.Observe(executableBase);
 ApplyParityPatches129();
 const bool local=runtimeConfig.mode==wht::RunMode::Local;
 if(!local&&mapReveal129.modifier){uintptr_t camera=0;uint16_t slot=0;if(CameraContext(camera,slot))mapReveal129.Set(executableBase,false);}
 for(const auto& message:wht::playerNetwork129.Drain())ParityNotice129(message);
 std::vector<unsigned char> saves;{std::lock_guard<std::mutex> guard(paritySaveMutex129);saves.swap(paritySaveEvents129);}
 if(local&&runtimeConfig.notiLogSaveGame)for(auto player:saves){
  std::string name="Player "+std::to_string(player+1);
  uintptr_t camera=0;uint16_t slot=0;if(resourceApi.Complete()&&CameraContext(camera,slot)){auto handle=resourceApi.player(player);auto object=handle?resourceApi.resolve(handle):0;auto text=object?resourceApi.name(object,1):nullptr;if(text&&*text)name=text;}
  ParityNotice129("Save Game: "+name);std::ofstream out(std::filesystem::path(runtimeFolder)/L"TS-SaveGame.log",std::ios::app);if(out)out<<"Save Game: "<<name<<'\n';
 }
 if(GetTickCount64()>=parityServicesDue129){
  parityServicesDue129=GetTickCount64()+1000;
  wht::playerNetwork129.Configure(runtimeConfig,runtimeFolder,wht::RegistryPlayerName129());RefreshScriptSettings129();
  if(local){
   if(runtimeConfig.refreshRate){HKEY key=nullptr;if(RegCreateKeyExW(HKEY_CURRENT_USER,L"Software\\Blizzard Entertainment\\Warcraft III\\Video",0,nullptr,0,KEY_SET_VALUE,nullptr,&key,nullptr)==ERROR_SUCCESS){DWORD rate=runtimeConfig.refreshRate;RegSetValueExW(key,L"refreshrate",0,REG_DWORD,reinterpret_cast<const BYTE*>(&rate),4);RegCloseKey(key);}}
   try{
    auto libraryFolder=std::filesystem::path(runtimeFolder)/L"WHT_Lib";
    if(std::filesystem::is_directory(libraryFolder))for(const auto& entry:std::filesystem::directory_iterator(libraryFolder))if(entry.is_regular_file()){
     auto extension=entry.path().extension().wstring(),stem=entry.path().stem().wstring();std::transform(extension.begin(),extension.end(),extension.begin(),towlower);std::transform(stem.begin(),stem.end(),stem.begin(),towlower);
     if(stem.find(L"fastjass")!=stem.npos&&!runtimeConfig.fastJass)continue;
     if(extension==L".dll")LoadParityLibrary129(entry.path(),false);else if(extension==L".mpq"||extension==L".mix")LoadParityLibrary129(entry.path(),true);
    }
    auto fast=std::filesystem::path(runtimeFolder)/L"FastJass.dll";if(runtimeConfig.fastJass&&std::filesystem::is_regular_file(fast))LoadParityLibrary129(fast,false);
   }catch(const std::exception& e){wht::Log(runtimeFolder,std::string("WHT 1.29 library load: ")+e.what());}
  }
 }
 const auto world=*reinterpret_cast<uintptr_t*>(executableBase+0xd3b6f4);
 uintptr_t activeCamera=0;uint16_t activeLocal=0;const bool match=local&&CameraContext(activeCamera,activeLocal);
 if(!match){parityTerrainWorld129=0;parityTerrainCode129.clear();terrainWork129.active=false;mapReveal129.modifier=0;mapReveal129.world=0;parityAppliedName129.clear();}
 else if(parityTerrainWorld129!=world||parityTerrainCode129!=runtimeConfig.codeNen){
  parityTerrainWorld129=world;parityTerrainCode129=runtimeConfig.codeNen;uint32_t code=0;
  if(!parityTerrainCode129.empty()&&wht::ParseRemoveAbility129(parityTerrainCode129,code))terrainWork129.Start(executableBase,code);
 }
 if(terrainWork129.Tick(executableBase,match))ParityNotice129("Terrain: complete");
 if(parityNameWorld129!=world){parityNameWorld129=world;parityAppliedName129.clear();}
 if(match&&(!runtimeConfig.newNames.empty()||!runtimeConfig.nameCreate.empty())){
  auto target=wht::Utf8(wht::playerNetwork129.IsHost()&&!runtimeConfig.nameCreate.empty()?runtimeConfig.nameCreate:runtimeConfig.newNames);
  if(!target.empty()&&target!=parityAppliedName129&&wht::RenameLocalPlayer129(executableBase,resourceApi,target.c_str()))parityAppliedName129=target;
 }
}
bool ParityCommand129(const std::wstring& name,std::wistringstream& in,bool& result){
 result=false;std::wstring value,extra;
 if(name==L"#kick"||name==L"#ban"||name==L"#openbl"){
  if(name==L"#openbl"){
   if(in>>extra)return true;auto path=runtimeFolder+L"\\TS-Banned.ini";
   if(GetFileAttributesW(path.c_str())==INVALID_FILE_ATTRIBUTES){std::ofstream out(std::filesystem::path(path),std::ios::binary);if(out)out<<"[Banned List]\r\n";}
   result=reinterpret_cast<INT_PTR>(ShellExecuteW(window,L"open",path.c_str(),nullptr,runtimeFolder.c_str(),SW_SHOWNORMAL))>32;
  }else if(name==L"#kick"){
   std::getline(in,value);value=wht::TrimParity129(value);if(value.size()>1&&value.front()==L'"'&&value.back()==L'"')value=value.substr(1,value.size()-2);
   ParityNotice129(wht::playerNetwork129.Kick(wht::Utf8(value)));result=true;
  }else{
   std::wstring reason;if(!(in>>std::quoted(value))){ParityNotice129("Usage: #ban <1..24 or exact name> [reason]");return true;}
   std::getline(in,reason);reason=wht::TrimParity129(reason);if(reason.empty())reason=L"No reason";
   ParityNotice129(wht::playerNetwork129.Ban(wht::Utf8(value),reason));result=true;
  }return true;
 }
 struct Toggle{const wchar_t* name;const wchar_t* section;const wchar_t* key;bool previous;const char* title;};
 const Toggle toggles[]={
  {L"#showmem",L"Event",L"Show Mem",runtimeConfig.showMem,"Show Memory"},{L"#mem",L"Event",L"Show Mem",runtimeConfig.showMem,"Show Memory"},
  {L"#memory",L"Event",L"Show Mem",runtimeConfig.showMem,"Show Memory"},{L"#ram",L"Event",L"Show Mem",runtimeConfig.showMem,"Show Memory"},{L"#usemem",L"Event",L"Show Mem",runtimeConfig.showMem,"Show Memory"},
  {L"#manabar",L"ManaBar",L"Enable",runtimeConfig.manaBarEnable,"Mana Bar"},{L"#mana",L"ManaBar",L"Enable",runtimeConfig.manaBarEnable,"Mana Bar"},{L"#showmana",L"ManaBar",L"Enable",runtimeConfig.manaBarEnable,"Mana Bar"},
  {L"#statetag",L"StateTag",L"Enable",runtimeConfig.stateTagEnable,"State Values"},{L"#hpmpvalue",L"StateTag",L"Enable",runtimeConfig.stateTagEnable,"State Values"},{L"#showmax",L"StateTag",L"ShowMaxValues",runtimeConfig.stateTagShowMax,"Show Maximum"},
  {L"#mth49",L"MTHKEY",L"MTH49Enable",runtimeConfig.mth49Enable,"MTH49"},{L"#g17",L"MTHKEY",L"G17Enable",runtimeConfig.g17Enable,"G17"},{L"#mthnoti",L"MTHKEY",L"Noti",runtimeConfig.mthNoti,"MTH Notifications"},
  {L"#hn",L"Event",L"Show Noti Kick/Ban",runtimeConfig.showNotiKickBan,"Kick/Ban Notifications"},{L"#cs",L"Event",L"Noti/Log Info Players Save Game",runtimeConfig.notiLogSaveGame,"Save Notifications"},{L"#csave",L"Event",L"Noti/Log Info Players Save Game",runtimeConfig.notiLogSaveGame,"Save Notifications"},
  {L"#vsync",L"Graphics",L"Vsync",runtimeConfig.vsync,"VSync (restart to apply)"},
  {L"#ssp",L"Event",L"Show Unit Skills",runtimeConfig.showUnitSkills,"Show Unit Skills"},{L"#showunitskills",L"Event",L"Show Unit Skills",runtimeConfig.showUnitSkills,"Show Unit Skills"},
  {L"#wide",L"Event",L"WideScreen",runtimeConfig.wide,"Wide Screen"},
  {L"#mapsize",L"Event",L"Unlock Map Size",runtimeConfig.unlockMap,"Unlock Map Size"},
  {L"#fastjass",L"Event",L"Fast Jass",runtimeConfig.fastJass,"Fast Jass"},{L"#fj",L"Event",L"Fast Jass",runtimeConfig.fastJass,"Fast Jass"}
 };
 for(const auto& t:toggles)if(name==t.name){
  if(!wht::ParityToggle129(in,t.previous,value)){ParityNotice129("Usage: "+wht::Utf8(name)+" [0|1]");return true;}
  if(value==L"1"&&(((!wcscmp(t.key,L"Show Unit Skills"))&&!inspectSkillsReady129)||(!wcscmp(t.key,L"Unlock Map Size")&&!mapSizeReady129))){ParityNotice129("This hook is unavailable for the current executable");return true;}
  if(value==L"1"&&(name==L"#mth49"||name==L"#g17"))if(!wht::SaveSetting(runtimeIni,L"MTHKEY",name==L"#mth49"?L"G17Enable":L"MTH49Enable",L"0"))return true;
  result=SaveParity129(t.section,t.key,value);if(result){
   if(name==L"#wide")ParityNotice129("Wide Screen: native 1.29 support; compatibility preference saved");
   else if(name==L"#fastjass"||name==L"#fj")ParityNotice129(value==L"1"?"Fast Jass: enabled; compatible FastJass.dll required":"Fast Jass: disabled for next launch (restart to unload DLL)");
   else ParityNotice129(std::string(t.title)+(value==L"1"?": ON":": OFF"));
  }return true;
 }
 if(name==L"#reloadconfig"){
  if(in>>extra){ParityNotice129("Usage: #reloadconfig");return true;}ReloadParity129();ParityNotice129("Configuration reloaded");result=true;return true;
 }
 if(name==L"#stopga"||name==L"#stopmth"){
  if(in>>extra){ParityNotice129("Usage: "+wht::Utf8(name));return true;}
  auto stop=wht::ParityNative129<int(__cdecl*)(uint32_t,uint32_t)>(executableBase,"IssueImmediateOrderById");
  if(stop&&mth129.ValidCourier(executableBase,mth129.courier,mth129.courierSession)){wht::MthAdapter129 api(executableBase,clickButton);auto handle=api.Handle(mth129.courier.unit);if(handle)stop(handle,0xd0004);}
  mth129.CancelActions();inventoryActions129.ToggleGoku(false);ParityNotice129("Courier/MTH: stopped");result=true;return true;
 }
 if(name==L"#pcam"||name==L"#pzoom"){
  float distance=2400;in>>std::ws;if(!in.eof()&&(!(in>>distance)||in>>extra||!std::isfinite(distance))){ParityNotice129("Usage: "+wht::Utf8(name)+" [distance]");return true;}
  distance=std::clamp(distance,500.f,6000.f);result=SaveParity129(L"Event",L"Camera",std::to_wstring(distance));if(result){ApplyCamera();ParityNotice129("Camera: "+std::to_string(int(distance)));}return true;
 }
 if(name==L"#dr"||name==L"#drn"||name==L"#cd"||name==L"#light"||name==L"#refreshrate"||name==L"#rr"||name==L"#arr"){
  const bool delay=name==L"#dr"||name==L"#drn",start=name==L"#cd",light=name==L"#light";
  if(((delay||start)&&!delayReady129)||(light&&!originalParityEndScene129)){ParityNotice129("This hook is unavailable for the current executable");return true;}
  const int low=delay?1:0,high=delay?1000:start?60:light?255:500;int n=0;
  if(!delay&&!start&&!light&&(in>>std::ws).eof()){
   DEVMODEW mode{};mode.dmSize=sizeof(mode);n=EnumDisplaySettingsW(nullptr,ENUM_CURRENT_SETTINGS,&mode)&&mode.dmDisplayFrequency>0?int(mode.dmDisplayFrequency):runtimeConfig.refreshRate?int(runtimeConfig.refreshRate):60;
   value=std::to_wstring(n);
  }else if(start){
   float seconds=0;if(!(in>>seconds)||in>>extra||!std::isfinite(seconds)||seconds<0||seconds>60){ParityNotice129("Usage: #cd <seconds 0..60>");return true;}
   result=SaveParity129(L"Event",L"Delay Start Game",std::to_wstring(seconds));if(result)ParityNotice129("Start delay: "+std::to_string(seconds)+" seconds");return true;
  }else
  if(!(in>>value)||in>>extra||!wht::ParityInteger129(value,low,high,n)){ParityNotice129("Usage: "+wht::Utf8(name)+" <"+std::to_string(low)+".."+std::to_string(high)+">");return true;}
  const wchar_t* key=name==L"#dr"?L"Local Game Delay Reducer":name==L"#drn"?L"Net Game Delay Reducer":start?L"Delay Start Game":light?L"Light":L"Refresh Rate";
  if(name==L"#dr"&&!wht::SaveSetting(runtimeIni,L"Event",L"Lan Game Delay Reducer",value))return true;
  result=SaveParity129(delay||start||light?L"Event":L"Graphics",key,value);if(result)ParityNotice129(wht::Utf8(name)+": "+wht::Utf8(value)+(delay||start||light?"":" (restart to apply)"));return true;
 }
 if(name==L"#name"||name==L"#n"||name==L"#oldname"||name==L"#namecreate"||name==L"#nc"){
  std::getline(in,value);value=wht::TrimParity129(value);const bool old=name==L"#oldname",host=name==L"#namecreate"||name==L"#nc";
  if(value.empty()){
   std::wstring current=old?runtimeConfig.oldNames:host?runtimeConfig.nameCreate:runtimeConfig.newNames;
   if(current.empty()&&!old&&!host)current=wht::RegistryPlayerName129();ParityNotice129(std::string(old?"Old Name: ":host?"Host Name: ":"Player Name: ")+wht::Utf8(current));result=true;return true;
  }
  bool restore=!old&&!host&&(value==L"restore"||value==L"reset"||value==L"default"||value==L"off");
  if(restore){value=runtimeConfig.oldNames;if(value.empty()){ParityNotice129("No original name recorded");return true;}}
  auto encoded=wht::Utf8(value);if(encoded.empty()||encoded.size()>63||value.find_first_of(L"\r\n")!=value.npos){ParityNotice129("Name: use 1..63 UTF-8 bytes");return true;}
  if(!old&&!host){
   if(runtimeConfig.oldNames.empty()){auto previous=wht::RegistryPlayerName129();if(!previous.empty()&&!wht::SaveSetting(runtimeIni,L"Event",L"OLDNames",previous))return true;}
   if(!wht::StorePlayerName129(value)){ParityNotice129("Could not save player name");return true;}
   if(!SaveParity129(L"Event",L"NewNames",restore?L"":value))return true;
   uintptr_t camera=0;uint16_t local=0;if(CameraContext(camera,local))wht::RenameLocalPlayer129(executableBase,resourceApi,encoded.c_str());parityAppliedName129.clear();
  }else if(!SaveParity129(L"Event",old?L"OLDNames":L"NameCreate",value))return true;
  ParityNotice129(std::string(old?"Old Name: ":host?"Host Name: ":"Player Name: ")+encoded);result=true;return true;
 }
 if(name==L"#war3map"){
  if(!originalScriptLoad129){ParityNotice129("Map script hook unavailable");return true;}
  std::wstring key=L"enable";if(in>>key){
   if(key==L"e")key=L"enable";if(key==L"l")key=L"load";
   if(key==L"file"||key==L"script"){
    std::getline(in,value);value=wht::TrimParity129(value);if(value.empty()){ParityNotice129("Usage: #war3map file <path.j>");return true;}
   }else if(key==L"load"&&(in>>std::ws).peek()!=WEOF){
    std::wstring argument;std::getline(in,argument);argument=wht::TrimParity129(argument);
    if(argument.size()>2&&argument.front()==L'"'&&argument.back()==L'"')argument=argument.substr(1,argument.size()-2);
    if(argument.size()>=2&&argument.substr(argument.size()-2)==L".j"){
     if(!SaveParity129(L"war3map",L"file",argument))return true;value=L"1";
    }else{std::wistringstream toggle(argument);if(!wht::ParityToggle129(toggle,runtimeConfig.war3mapLoad,value))return true;}
   }else if(key==L"enable"||key==L"load"){
    if(!wht::ParityToggle129(in,key==L"enable"?runtimeConfig.war3mapEnable:runtimeConfig.war3mapLoad,value))return true;
   }else {ParityNotice129("Usage: #war3map [enable|load [0|1]|file <path.j>]");return true;}
  }else value=runtimeConfig.war3mapEnable?L"0":L"1";
  result=SaveParity129(L"war3map",key.c_str(),value);if(result)ParityNotice129("war3map "+wht::Utf8(key)+": "+wht::Utf8(value)+" (next map)");return true;
 }
 if(name==L"#mh"){
  uintptr_t camera=0;uint16_t local=0;if(!CameraContext(camera,local)){ParityNotice129("Map Reveal: an active match is required");return true;}
  if(!wht::ParityToggle129(in,mapReveal129.modifier!=0,value))return true;
  result=mapReveal129.Set(executableBase,value==L"1");ParityNotice129(result?(value==L"1"?"Map Reveal: ON":"Map Reveal: OFF"):"Map Reveal: native unavailable or no active match");return true;
 }
 if(name==L"#mtrade"||name==L"#q"){
  if(in>>extra)return true;uintptr_t camera=0;uint16_t local=0;if(!CameraContext(camera,local)){ParityNotice129("An active match is required");return true;}
  if(name==L"#mtrade"){auto fn=wht::ParityNative129<void(__cdecl*)(int,int)>(executableBase,"SetMapFlag");if(fn){fn(0x100,0);result=true;ParityNotice129("Trade restriction disabled");}}
  else{auto fn=wht::ParityNative129<void(__cdecl*)(int)>(executableBase,"EndGame");if(fn){fn(0);result=true;}}return true;
 }
 if(name==L"#codenen"||name==L"#code_nen"||name==L"#nen"||name==L"#addall"||name==L"#raddall"){
  const bool terrain=name!=L"#addall"&&name!=L"#raddall";
  if(terrain&&(in>>std::ws).eof()){ParityNotice129("Terrain code: "+(runtimeConfig.codeNen.empty()?std::string("default"):wht::Utf8(runtimeConfig.codeNen)));result=true;return true;}
  if(terrain){auto position=in.tellg();in>>value;if(value==L"0"||value==L"off"||value==L"none"||value==L"clear"){
   if(in>>extra)return true;result=SaveParity129(L"Event",L"code_nen",L"");if(result){terrainWork129.active=false;ParityNotice129("Terrain: disabled for next map (current tiles retained)");}return true;
  }in.clear();in.seekg(position);}
  uint32_t code=0;if(!(in>>value)||in>>extra||!wht::ParseRemoveAbility129(value,code)){ParityNotice129("Usage: "+wht::Utf8(name)+" <rawcode>");return true;}
  if(name==L"#addall"||name==L"#raddall"){
   uintptr_t camera=0;uint16_t local=0;if(!CameraContext(camera,local)){ParityNotice129("An active match is required");return true;}
   auto changed=wht::ChangeOwnedAbilities129(executableBase,code,name==L"#addall");result=changed>0;ParityNotice129("Ability "+wht::Utf8(value)+": "+std::to_string(changed)+" owned units changed");
  }else{if(!SaveParity129(L"Event",L"code_nen",value))return true;uintptr_t camera=0;uint16_t local=0;
   const bool match=CameraContext(camera,local);result=!match||terrainWork129.Start(executableBase,code);
   parityTerrainWorld129=match?*reinterpret_cast<uintptr_t*>(executableBase+0xd3b6f4):0;parityTerrainCode129=match?value:L"";
   ParityNotice129(match?(result?"Terrain: queued":"Terrain: native unavailable"):"Terrain: saved for next match");}return true;
 }
 if(name==L"#ufind"||name==L"#ufinds"){
  if(!(in>>value)||in>>extra){ParityNotice129("Usage: "+wht::Utf8(name)+" <rawcode/name>");return true;}
  wht::UnitInfo129 info;if(!wht::ReadUnitInfo129(executableBase,true,info)){ParityNotice129("Select a unit first");return true;}
  if(name==L"#ufind"){uint32_t code=0;if(!wht::ParseRemoveAbility129(value,code)||code!=info.code){ParityNotice129("Unit not found in active selection");return true;}}
  ParityNotice129("Selected "+wht::Utf8(value)+": "+wht::UnitPositionText129(info.x,info.y));result=true;return true;
 }
 if(name==L"#emodel"||name==L"#smodel"){
  std::getline(in,value);value=wht::TrimParity129(value);if(value.size()>259){ParityNotice129("Usage: "+wht::Utf8(name)+" <model path>");return true;}
  customModelPath129=value;ParityNotice129("Model Path: "+wht::Utf8(value)+" (stored, same behavior as 1.24/1.26)");result=true;return true;
 }
 if(name==L"#gmodel"){
  if(in>>extra)return true;wht::UnitInfo129 info;if(!wht::ReadUnitInfo129(executableBase,false,info)){ParityNotice129("Select a unit first");return true;}
  ParityNotice129(customModelPath129.empty()?"Model: native default | "+wht::UnitCodeText129(info.code):"Model Path: "+wht::Utf8(customModelPath129));result=true;return true;
 }
 return false;
}
