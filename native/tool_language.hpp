#pragma once

inline std::string ToolTabName(size_t page){
 static const char* en[]={"General","Quick Chat","Hotkeys","Other Hotkeys","Graphics","MTH","Automation","Language"};
 return toolEnglish.load()?en[page]:kToolMenuTabs[page];
}
inline std::string EnglishTabHelp(size_t page){
 static const char* descriptions[]={
  "Configure in-game displays and utility buttons.",
  "Assign hotkeys and messages for quick chat.",
  "Assign skill, inventory and hero-selection hotkeys.",
  "Configure utility hotkeys, casting mode, overhead text and window settings.",
  "Configure interface visibility, colors, FPS and camera scroll speeds.",
  "Select the MTH mode and configure MTH actions and hotkeys.",
  "Configure automatic skills and macro recording, playback and hotkeys.",
  "Select Vietnamese or English for the tools menu and tooltips."
 };
 return ToolTabName(page)+"\n"+descriptions[page];
}
inline std::string EnglishControlLabel(const ToolMenuControl& d){
 std::wstring key=d.key,section=d.section;
 if(key==L"Keyboard Scroll Speed")return "Keyboard camera scroll (%)";
 if(key==L"Mouse Scroll Speed")return "Mouse camera scroll (%)";
 if(key==L"Borderless")return "Borderless window";
 if(key==L"Lock Cursor")return "Lock cursor in game";
 if(key==L"Sound Start Game")return "Play match-start sound";
 if(key==L"ModeCast")return "Casting mode";
 if(key==L"Fast Menu")return "Faster game menu transitions";
 if(section==L"StateTag"&&key==L"ShowHpValue")return "Show overhead HP value";
 if(section==L"StateTag"&&key==L"FontSize")return "Overhead HP/Mana font size";
 if(section==L"MacroRecorder")return key==L"Enabled"?"Enable Macro Recorder":key==L"ShowButton"?"Show Macro button":key==L"PlayKey"?"Run hotkey":key==L"StopKey"?"Stop hotkey":key==L"RecordKey"?"Record hotkey":"Script hotkey";
 if(section==L"Macro")return key==L"record"?"Record / Stop recording":key==L"play"?"Play macro":key==L"stop"?"Stop macro":"Edit script";
 if(key==L"Vietnamese")return "Vietnamese";if(key==L"English")return "English";
 if(d.kind==MenuControlKind::Label&&std::string(d.label).find("BackSpace")!=std::string::npos)return "(Press Backspace to clear)";
 if(key==L"Enable")return section==L"ManaBar"?"Mana bar":section==L"StateTag"?"HP/Mana value":section==L"Quick Chat"?"Enable Quick Chat":section==L"SlotHotkeyInv"?"Enable inventory hotkeys":section==L"SlotHotkeyShop"?"Enable learn/shop/spellbook hotkeys":"Enable skill hotkeys";
 if(key==L"EnableForHero")return "Show for heroes";if(key==L"EnableForNonHero")return "Show for units";
 struct Label{const wchar_t* key;const char* text;};
 static const Label labels[]={
 {L"ShowCooldown","Cooldown time"},{L"ShowRegenHP","Show HP regeneration/s"},{L"ShowRegenMana","Show mana regeneration/s"},
 {L"ShowMaxValues","Show maximum HP/Mana"},{L"ShowManaValue","Show mana value"},{L"ShowLevelHero","Show hero level"},
 {L"Fog Click","Enable Fog Click"},{L"Show Hotkey In Panel","Show hotkeys on buttons"},
 {L"Enable Delay Clear Text Screen","Periodically clear screen text"},{L"Enable Delay Clear Chat Screen","Periodically clear chat"},
 {L"Show Resource","Show resource table"},{L"Quick Drop Item","Quick drop item (Shift + right click)"},
 {L"Show Timer","Show match time"},{L"Show FPS","Show FPS"},{L"Show Mem","Show memory usage"},
 {L"Show Auto Pick","Show Auto Pick button"},{L"Show Drop All","Show Drop All button"},{L"Show Sell All","Show Sell All button"},{L"Show Ins All Skills","Show All Skills button"},
 {L"hko_MinimizeMultiboard","Collapse/expand multiboard"},{L"hko_menu","Open/close menu"},{L"hko_command","Pause/restore command settings"},
 {L"Text Life/Mana Commas","HP/Mana thousands separators"},{L"Text Life/Mana No Hide","Keep long HP/Mana values visible"},{L"Text Stats Commas","Stats thousands separators"},{L"Detailed Stats Tooltip","Detailed damage/armor tooltips"},
 {L"Color Player","Player-colored health bars"},{L"VSync","Enable VSync"},{L"BarInvul","Hide bars for invulnerable units"},
 {L"ConsoleUI","Hide Console UI"},{L"BlackBarUI","Hide Black Bar UI"},{L"timeOfDayUI","Hide Time of Day UI"},{L"MouseTooltipUI","Tooltip follows mouse"},{L"VertexColor","Force opaque vertex color"},
 {L"MTH49Enable","MTH 49"},{L"G17Enable","G17"},{L"Noti","Show notifications"},{L"TimeBoss","Boss timers"},
 {L"Shield","Craft Shield"},{L"Ice","Craft Ice"},{L"Wild","Craft Wild"},{L"Frozen","Craft Frozen"},{L"Phoenix","Craft Phoenix"},{L"PickGoku","Pick up Goku"},
 {L"ShieldDraw","Show Shield button"},{L"IceDraw","Show Ice button"},{L"WildDraw","Show Wild button"},{L"FrozenDraw","Show Frozen button"},{L"PhoenixDraw","Show Phoenix button"},{L"PickGokuDraw","Show Goku button"},
 {L"AutoSkills_Enable","Enable Auto Skills"},{L"AutoSkills_Delay","Cast delay"},{L"AutoSkills_Range","Target scan range"},{L"AutoSkills_Active","Start/stop hotkey"},
 {L"SellFast","Quick Sell"},{L"SetIdTarget","Set target unit"},{L"AutoClickLeft","Auto left click"},{L"AutoClickRight","Auto right click"}
 };
 for(const auto& item:labels)if(key==item.key)return item.text;
 if(key.rfind(L"ChayGa",0)==0)return "Chicken route "+Utf8(key.substr(6,1));
 if(key.rfind(L"ChaySoi",0)==0)return "Wolf route "+Utf8(key.substr(7,1));
 if(key.rfind(L"AutoGaDraw",0)==0)return "Show Chicken "+Utf8(key.substr(10))+" button";
 if(key.rfind(L"AutoSoiDraw",0)==0)return "Show Wolf "+Utf8(key.substr(11))+" button";
 if(!*d.label)return {};
 // The remaining descriptor labels are already English (hero keys, Clear, legend).
 return d.label;
}
inline std::string EnglishControlHelp(const ToolMenuControl& d){
 if(!wcscmp(d.key,L"Keyboard Scroll Speed")||!wcscmp(d.key,L"Mouse Scroll Speed"))return "Camera scroll speed\n0..700%; default 100%. Enter a value and press Enter to save.";
 if(!wcscmp(d.key,L"Borderless"))return "Borderless window\nFill the screen without a window border. Disable to restore the previous window.";
 if(!wcscmp(d.key,L"Lock Cursor"))return "Lock cursor in game\nKeep the cursor inside the active game window. Release it when the game loses focus.";
 if(!wcscmp(d.key,L"Sound Start Game"))return "Match-start sound\nPlay once at the start of a new match. Enabling during a match does not play a sound.";

 if(!wcscmp(d.key,L"Fast Menu"))return "Fast Menu\nShorten the game menu transition wait. Original game actions and callbacks are retained.";
 if(!wcscmp(d.section,L"StateTag")&&!wcscmp(d.key,L"ShowHpValue"))return "Overhead HP value\nShow or hide HP numbers independently of mana. Enable HP/Mana values in General first.";
 if(!wcscmp(d.section,L"StateTag")&&!wcscmp(d.key,L"FontSize"))return "Overhead HP/Mana font size\nRange 1 to 20; default 4. Changes existing labels and line spacing immediately.";
 if(!wcscmp(d.key,L"ModeCast"))return "Casting mode\nQuick Cast: on press. Smart Cast: on release. Enable Quick Cast for each slot first.";
 std::wstring key=d.key,section=d.section;auto title=EnglishControlLabel(d);std::string help;
 auto starts=[&](const wchar_t* p){return key.rfind(p,0)==0;};
 auto digit=key.find_first_of(L"0123456789");auto slot=digit==std::wstring::npos?1:1+int(wcstol(key.c_str()+digit,nullptr,10));
 if(section==L"MacroRecorder"){return EnglishControlLabel(d)+"\n"+(key==L"Enabled"?"Enable recording, playback and macro hotkeys.":key==L"ShowButton"?"Show the Macro icon and action buttons.":"Assign a key for this macro action. Backspace clears it; Escape cancels.");}
 if(section==L"Macro"){help=key==L"record"?"Record mouse and keyboard (Ctrl+Shift+F9). A new recording replaces the saved script.":key==L"play"?"Replay the saved script with the configured repeat count (Ctrl+Shift+F10). Playback stops when the game loses focus.":key==L"stop"?"Stop recording or playback (Ctrl+Shift+F12).": "Open the script editor (Ctrl+Shift+F11). Add, edit or delete lines, then validate and save.";}
 else if(section==L"Language"){title="Language";help="Select Vietnamese or English for the tool interface and tooltips. The choice is saved automatically.";}
 else if(section==L"Quick Chat"){
  if(key==L"Enable")help="Enable or disable all saved quick-chat hotkeys.";
  else {title="Quick-chat message "+Utf8(key.substr(digit));help=d.kind==MenuControlKind::Key?"Press the assigned key to send this row's saved message.":d.kind==MenuControlKind::Clear?"Clear the message in this row.":"Edit the message sent by this row's hotkey.";}
 }else if(section==L"SlotHotkey"||section==L"SlotHotkeyShop"||section==L"SlotHotkeyInv"){
  if(key==L"Enable")help="Enable or disable this entire hotkey group. Saved slot settings are preserved.";
  else {title=(section==L"SlotHotkeyInv"?"Inventory slot ":section==L"SlotHotkeyShop"?"Learn/shop/spellbook slot ":"Command slot ")+std::to_string(slot);
   help=starts(L"QuickCast")?"Cast immediately at the mouse position when the assigned key is pressed.":starts(L"AutoTarget")?"Use this skill or item on your hero when the assigned key is pressed.":d.kind==MenuControlKind::Check?"Enable or disable the hotkey for this slot.":"Press the assigned key to activate the button in this slot.";}
 }else if(section==L"MTHKEY"){
  help=d.kind==MenuControlKind::Choice?"Choose how many movement orders to queue with Shift for this route.":d.kind==MenuControlKind::Key?"Press the assigned key to perform this MTH crafting, pickup or movement action.":key==L"TimeBoss"?"Show the boss schedule and Human/Undead respawn countdowns in match time.":key==L"MTH49Enable"||key==L"G17Enable"?"Select this MTH mode and disable the other mode.":key==L"Noti"?"Show feedback for MTH actions.":"Show or hide the corresponding MTH action button during a match.";
 }else if(starts(L"HeroF")){title="Select hero "+Utf8(key.substr(5));help="Press to select this hero; press twice to center the camera on the hero.";
 }else if(key==L"SellFast")help="Hover an inventory item and press the assigned key to sell that item at a nearby shop.";
 else if(key==L"hko_command")help="Pause camera, fog, widescreen, scroll and delay overrides. Press again to restore saved settings.";
 else if(key==L"hko_menu")help="Press the assigned key to open or close the in-game tools menu.";
 else if(key==L"hko_MinimizeMultiboard")help="Press the assigned key to collapse or expand the map's information board.";
 else if(key==L"AutoClickLeft"||key==L"AutoClickRight")help="Press the assigned key to start or stop repeated clicks at the mouse position.";
 else if(key==L"SetIdTarget")help="Record the selected unit as the target for supported targeting actions.";
 else if(key==L"AutoSkills_Enable")help="Enable skill sequences. Hover a skill and press Ctrl+Alt+1..9 to assign a step for the selected unit.";
 else if(key==L"AutoSkills_Active")help="Press the assigned key to start or stop the selected unit's skill sequence.";
 else if(key==L"AutoSkills_Delay")help="Delay between skill attempts, measured in match seconds.";
 else if(key==L"AutoSkills_Range")help="Scan radius for the nearest visible living enemy.";
 else if(key==L"Show Sell All")help="Show the button that sells all carried items at a shop (#sellall).";
 else if(key==L"Show Drop All")help="Show the button that drops all carried items (#dropall).";
 else if(key==L"Show Ins All Skills")help="Show the button that spends available hero skill points (#allskills).";
 else if(key==L"Show Auto Pick")help="Show the button that toggles automatic pickup of nearby items.";
 else if(key==L"ShowMaxValues")help="Append maximum values after current HP/Mana. Requires HP/Mana value display.";
 else if(section==L"DelayClear"){title=key.find(L"Chat")!=std::wstring::npos?"Clear chat":"Clear screen text";help=d.kind==MenuControlKind::Choice?"Choose the interval between clears, in seconds.":"Periodically clear the displayed messages.";}
 else help="Enable or disable "+title+". Saved settings are preserved.";
 if(title.empty())title="Tool option";
 if(d.kind==MenuControlKind::Key)help+="\nClick the box, then press a key to bind. Backspace clears; Esc cancels.";
 if(d.kind==MenuControlKind::Text||d.kind==MenuControlKind::Number)help+="\nClick to edit. Enter saves; Esc cancels.";
 if(!d.supported)help+="\nThis feature is not available yet.";
 return title+"\n"+help;
}
