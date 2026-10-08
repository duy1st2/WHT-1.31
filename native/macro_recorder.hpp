#pragma once
#include "common.hpp"
#include <sstream>
#include <fstream>
#include <filesystem>
#include <set>
#include <algorithm>
#include <commctrl.h>
namespace wht {
struct MacroStep{std::string op;int a=0,b=0;};
inline bool ParseMacro(const std::string& text,std::vector<MacroStep>& steps,std::string& error){
 steps.clear();std::istringstream lines(text);std::string line;unsigned number=0;uint64_t duration=0;
 while(std::getline(lines,line)){++number;auto comment=line.find('#');if(comment!=std::string::npos)line.erase(comment);std::istringstream in(line);MacroStep s;if(!(in>>s.op))continue;
  bool valid=true;std::string extra;
  if(s.op=="WAIT"){valid=bool(in>>s.a)&&s.a>=0&&s.a<=600000;duration+=unsigned(std::max(0,s.a));}
  else if(s.op=="MOVE")valid=bool(in>>s.a>>s.b)&&s.a>=0&&s.a<=10000&&s.b>=0&&s.b<=10000;
  else if(s.op=="KEYDOWN"||s.op=="KEYUP")valid=bool(in>>s.a)&&s.a>=8&&s.a<=254;
  else if(s.op=="WHEEL")valid=bool(in>>s.a)&&s.a>=-12000&&s.a<=12000;
  else valid=s.op=="LDOWN"||s.op=="LUP"||s.op=="RDOWN"||s.op=="RUP"||s.op=="MDOWN"||s.op=="MUP";
  if(!valid||(in>>extra)||duration>600000||steps.size()>=50000){error="Line "+std::to_string(number)+": invalid action, arguments or limit (10 minutes / 50000 actions).";steps.clear();return false;}
  steps.push_back(s);
 }
 error.clear();return true;
}
struct MacroRecorder{
 unsigned repeatCount=1,completedLoops=0;HWND grid=nullptr,operation=nullptr,argA=nullptr,argB=nullptr,repeatEdit=nullptr,keyPicker=nullptr,labelA=nullptr,labelB=nullptr;std::vector<MacroStep> editorSteps;
 bool recording=false,playing=false;std::string script,status;std::wstring file;HWND game=nullptr,editor=nullptr,edit=nullptr,statusLabel=nullptr;
 std::vector<MacroStep> steps;size_t cursor=0;unsigned recordedActions=0;ULONGLONG started=0,last=0,next=0,lastMove=0;std::set<int> held;unsigned mouseHeld=0;
 std::set<int> recordedHeld;unsigned recordedMouse=0;
 using InputFn=UINT(WINAPI*)(UINT,LPINPUT,int);InputFn input=SendInput;using ForegroundFn=HWND(WINAPI*)();ForegroundFn foreground=GetForegroundWindow;
 bool English()const{return toolEnglish.load();}
 void Configure(HWND window,const std::wstring& folder){game=window;file=folder+L"\\WHT.macro.txt";}
 std::string GameStatus()const{auto text=Wide(status);for(auto& c:text){switch(c){
 case 0x00c0:c=L'A';break;
 case 0x00c1:c=L'A';break;
 case 0x00c2:c=L'A';break;
 case 0x00c3:c=L'A';break;
 case 0x00c4:c=L'A';break;
 case 0x00c5:c=L'A';break;
 case 0x00c7:c=L'C';break;
 case 0x00c8:c=L'E';break;
 case 0x00c9:c=L'E';break;
 case 0x00ca:c=L'E';break;
 case 0x00cb:c=L'E';break;
 case 0x00cc:c=L'I';break;
 case 0x00cd:c=L'I';break;
 case 0x00ce:c=L'I';break;
 case 0x00cf:c=L'I';break;
 case 0x00d1:c=L'N';break;
 case 0x00d2:c=L'O';break;
 case 0x00d3:c=L'O';break;
 case 0x00d4:c=L'O';break;
 case 0x00d5:c=L'O';break;
 case 0x00d6:c=L'O';break;
 case 0x00d9:c=L'U';break;
 case 0x00da:c=L'U';break;
 case 0x00db:c=L'U';break;
 case 0x00dc:c=L'U';break;
 case 0x00dd:c=L'Y';break;
 case 0x00e0:c=L'a';break;
 case 0x00e1:c=L'a';break;
 case 0x00e2:c=L'a';break;
 case 0x00e3:c=L'a';break;
 case 0x00e4:c=L'a';break;
 case 0x00e5:c=L'a';break;
 case 0x00e7:c=L'c';break;
 case 0x00e8:c=L'e';break;
 case 0x00e9:c=L'e';break;
 case 0x00ea:c=L'e';break;
 case 0x00eb:c=L'e';break;
 case 0x00ec:c=L'i';break;
 case 0x00ed:c=L'i';break;
 case 0x00ee:c=L'i';break;
 case 0x00ef:c=L'i';break;
 case 0x00f1:c=L'n';break;
 case 0x00f2:c=L'o';break;
 case 0x00f3:c=L'o';break;
 case 0x00f4:c=L'o';break;
 case 0x00f5:c=L'o';break;
 case 0x00f6:c=L'o';break;
 case 0x00f9:c=L'u';break;
 case 0x00fa:c=L'u';break;
 case 0x00fb:c=L'u';break;
 case 0x00fc:c=L'u';break;
 case 0x00fd:c=L'y';break;
 case 0x00ff:c=L'y';break;
 case 0x0100:c=L'A';break;
 case 0x0101:c=L'a';break;
 case 0x0102:c=L'A';break;
 case 0x0103:c=L'a';break;
 case 0x0104:c=L'A';break;
 case 0x0105:c=L'a';break;
 case 0x0106:c=L'C';break;
 case 0x0107:c=L'c';break;
 case 0x0108:c=L'C';break;
 case 0x0109:c=L'c';break;
 case 0x010a:c=L'C';break;
 case 0x010b:c=L'c';break;
 case 0x010c:c=L'C';break;
 case 0x010d:c=L'c';break;
 case 0x010e:c=L'D';break;
 case 0x010f:c=L'd';break;
 case 0x0110:c=L'D';break;
 case 0x0111:c=L'd';break;
 case 0x0112:c=L'E';break;
 case 0x0113:c=L'e';break;
 case 0x0114:c=L'E';break;
 case 0x0115:c=L'e';break;
 case 0x0116:c=L'E';break;
 case 0x0117:c=L'e';break;
 case 0x0118:c=L'E';break;
 case 0x0119:c=L'e';break;
 case 0x011a:c=L'E';break;
 case 0x011b:c=L'e';break;
 case 0x011c:c=L'G';break;
 case 0x011d:c=L'g';break;
 case 0x011e:c=L'G';break;
 case 0x011f:c=L'g';break;
 case 0x0120:c=L'G';break;
 case 0x0121:c=L'g';break;
 case 0x0122:c=L'G';break;
 case 0x0123:c=L'g';break;
 case 0x0124:c=L'H';break;
 case 0x0125:c=L'h';break;
 case 0x0128:c=L'I';break;
 case 0x0129:c=L'i';break;
 case 0x012a:c=L'I';break;
 case 0x012b:c=L'i';break;
 case 0x012c:c=L'I';break;
 case 0x012d:c=L'i';break;
 case 0x012e:c=L'I';break;
 case 0x012f:c=L'i';break;
 case 0x0130:c=L'I';break;
 case 0x0134:c=L'J';break;
 case 0x0135:c=L'j';break;
 case 0x0136:c=L'K';break;
 case 0x0137:c=L'k';break;
 case 0x0139:c=L'L';break;
 case 0x013a:c=L'l';break;
 case 0x013b:c=L'L';break;
 case 0x013c:c=L'l';break;
 case 0x013d:c=L'L';break;
 case 0x013e:c=L'l';break;
 case 0x0143:c=L'N';break;
 case 0x0144:c=L'n';break;
 case 0x0145:c=L'N';break;
 case 0x0146:c=L'n';break;
 case 0x0147:c=L'N';break;
 case 0x0148:c=L'n';break;
 case 0x014c:c=L'O';break;
 case 0x014d:c=L'o';break;
 case 0x014e:c=L'O';break;
 case 0x014f:c=L'o';break;
 case 0x0150:c=L'O';break;
 case 0x0151:c=L'o';break;
 case 0x0154:c=L'R';break;
 case 0x0155:c=L'r';break;
 case 0x0156:c=L'R';break;
 case 0x0157:c=L'r';break;
 case 0x0158:c=L'R';break;
 case 0x0159:c=L'r';break;
 case 0x015a:c=L'S';break;
 case 0x015b:c=L's';break;
 case 0x015c:c=L'S';break;
 case 0x015d:c=L's';break;
 case 0x015e:c=L'S';break;
 case 0x015f:c=L's';break;
 case 0x0160:c=L'S';break;
 case 0x0161:c=L's';break;
 case 0x0162:c=L'T';break;
 case 0x0163:c=L't';break;
 case 0x0164:c=L'T';break;
 case 0x0165:c=L't';break;
 case 0x0168:c=L'U';break;
 case 0x0169:c=L'u';break;
 case 0x016a:c=L'U';break;
 case 0x016b:c=L'u';break;
 case 0x016c:c=L'U';break;
 case 0x016d:c=L'u';break;
 case 0x016e:c=L'U';break;
 case 0x016f:c=L'u';break;
 case 0x0170:c=L'U';break;
 case 0x0171:c=L'u';break;
 case 0x0172:c=L'U';break;
 case 0x0173:c=L'u';break;
 case 0x0174:c=L'W';break;
 case 0x0175:c=L'w';break;
 case 0x0176:c=L'Y';break;
 case 0x0177:c=L'y';break;
 case 0x0178:c=L'Y';break;
 case 0x0179:c=L'Z';break;
 case 0x017a:c=L'z';break;
 case 0x017b:c=L'Z';break;
 case 0x017c:c=L'z';break;
 case 0x017d:c=L'Z';break;
 case 0x017e:c=L'z';break;
 case 0x1ea0:c=L'A';break;
 case 0x1ea1:c=L'a';break;
 case 0x1ea2:c=L'A';break;
 case 0x1ea3:c=L'a';break;
 case 0x1ea4:c=L'A';break;
 case 0x1ea5:c=L'a';break;
 case 0x1ea6:c=L'A';break;
 case 0x1ea7:c=L'a';break;
 case 0x1ea8:c=L'A';break;
 case 0x1ea9:c=L'a';break;
 case 0x1eaa:c=L'A';break;
 case 0x1eab:c=L'a';break;
 case 0x1eac:c=L'A';break;
 case 0x1ead:c=L'a';break;
 case 0x1eae:c=L'A';break;
 case 0x1eaf:c=L'a';break;
 case 0x1eb0:c=L'A';break;
 case 0x1eb1:c=L'a';break;
 case 0x1eb2:c=L'A';break;
 case 0x1eb3:c=L'a';break;
 case 0x1eb4:c=L'A';break;
 case 0x1eb5:c=L'a';break;
 case 0x1eb6:c=L'A';break;
 case 0x1eb7:c=L'a';break;
 case 0x1eb8:c=L'E';break;
 case 0x1eb9:c=L'e';break;
 case 0x1eba:c=L'E';break;
 case 0x1ebb:c=L'e';break;
 case 0x1ebc:c=L'E';break;
 case 0x1ebd:c=L'e';break;
 case 0x1ebe:c=L'E';break;
 case 0x1ebf:c=L'e';break;
 case 0x1ec0:c=L'E';break;
 case 0x1ec1:c=L'e';break;
 case 0x1ec2:c=L'E';break;
 case 0x1ec3:c=L'e';break;
 case 0x1ec4:c=L'E';break;
 case 0x1ec5:c=L'e';break;
 case 0x1ec6:c=L'E';break;
 case 0x1ec7:c=L'e';break;
 case 0x1ec8:c=L'I';break;
 case 0x1ec9:c=L'i';break;
 case 0x1eca:c=L'I';break;
 case 0x1ecb:c=L'i';break;
 case 0x1ecc:c=L'O';break;
 case 0x1ecd:c=L'o';break;
 case 0x1ece:c=L'O';break;
 case 0x1ecf:c=L'o';break;
 case 0x1ed0:c=L'O';break;
 case 0x1ed1:c=L'o';break;
 case 0x1ed2:c=L'O';break;
 case 0x1ed3:c=L'o';break;
 case 0x1ed4:c=L'O';break;
 case 0x1ed5:c=L'o';break;
 case 0x1ed6:c=L'O';break;
 case 0x1ed7:c=L'o';break;
 case 0x1ed8:c=L'O';break;
 case 0x1ed9:c=L'o';break;
 case 0x1eda:c=L'O';break;
 case 0x1edb:c=L'o';break;
 case 0x1edc:c=L'O';break;
 case 0x1edd:c=L'o';break;
 case 0x1ede:c=L'O';break;
 case 0x1edf:c=L'o';break;
 case 0x1ee0:c=L'O';break;
 case 0x1ee1:c=L'o';break;
 case 0x1ee2:c=L'O';break;
 case 0x1ee3:c=L'o';break;
 case 0x1ee4:c=L'U';break;
 case 0x1ee5:c=L'u';break;
 case 0x1ee6:c=L'U';break;
 case 0x1ee7:c=L'u';break;
 case 0x1ee8:c=L'U';break;
 case 0x1ee9:c=L'u';break;
 case 0x1eea:c=L'U';break;
 case 0x1eeb:c=L'u';break;
 case 0x1eec:c=L'U';break;
 case 0x1eed:c=L'u';break;
 case 0x1eee:c=L'U';break;
 case 0x1eef:c=L'u';break;
 case 0x1ef0:c=L'U';break;
 case 0x1ef1:c=L'u';break;
 case 0x1ef2:c=L'Y';break;
 case 0x1ef3:c=L'y';break;
 case 0x1ef4:c=L'Y';break;
 case 0x1ef5:c=L'y';break;
 case 0x1ef6:c=L'Y';break;
 case 0x1ef7:c=L'y';break;
 case 0x1ef8:c=L'Y';break;
 case 0x1ef9:c=L'y';break;
 default:break;}}return Utf8(text);}
 void SetStatus(const std::string& text){status=text;if(statusLabel)SetWindowTextW(statusLabel,Wide(text).c_str());}
 bool Save(){std::ofstream out(std::filesystem::path(file),std::ios::binary|std::ios::trunc);out<<script;if(!out){SetStatus(English()?"Cannot save macro file.":Utf8(L"Không lưu được file macro."));return false;}return true;}
 void Stop(){recording=false;playing=false;std::vector<INPUT> inputs;
  for(int vk:held){INPUT i{};i.type=INPUT_KEYBOARD;i.ki.wVk=WORD(vk);i.ki.dwFlags=KEYEVENTF_KEYUP;inputs.push_back(i);}held.clear();
  for(unsigned n=0;n<3;++n)if(mouseHeld&(1u<<n)){INPUT i{};i.type=INPUT_MOUSE;i.mi.dwFlags=n==0?MOUSEEVENTF_LEFTUP:n==1?MOUSEEVENTF_RIGHTUP:MOUSEEVENTF_MIDDLEUP;inputs.push_back(i);}mouseHeld=0;
  if(!inputs.empty())input(UINT(inputs.size()),inputs.data(),sizeof(INPUT));
 }
 void FinishRecording(){recording=false;for(int key:recordedHeld)script+="KEYUP "+std::to_string(key)+"\r\n";recordedHeld.clear();for(unsigned n=0;n<3;++n)if(recordedMouse&(1u<<n))script+=n==0?"LUP\r\n":n==1?"RUP\r\n":"MUP\r\n";recordedMouse=0;if(!Save())return;SetStatus(English()?"Recording saved. Open the script editor to edit actions.":Utf8(L"Đã lưu bản ghi. Mở trình sửa script để chỉnh các thao tác."));}
 void Record(){if(recording){FinishRecording();return;}Stop();script="# REPEAT "+std::to_string(repeatCount)+"\r\n# WHT macro: WAIT ms; MOVE x y (0..10000); KEYDOWN/KEYUP virtual-key; LDOWN/LUP; RDOWN/RUP; MDOWN/MUP; WHEEL delta\r\n";last=GetTickCount64();started=last;recordedActions=0;lastMove=0;recording=true;SetStatus(English()?"Recording mouse and keyboard...":Utf8(L"Đang ghi chuột và bàn phím..."));}
 bool Load(){std::ifstream in(std::filesystem::path(file),std::ios::binary);if(!in){SetStatus(English()?"No saved macro. Record one first.":Utf8(L"Chưa có macro. Hãy ghi trước."));return false;}std::ostringstream out;out<<in.rdbuf();script=out.str();repeatCount=1;std::istringstream header(script);std::string line;while(std::getline(header,line)){if(!line.empty()&&line.back()==char(13))line.pop_back();if(line.rfind("# REPEAT ",0)==0){auto value=line.substr(9);if(value.empty()||value.size()>7||value.find_first_not_of("0123456789")!=std::string::npos||std::stoul(value)>1000000){SetStatus("Invalid repeat count (0..1000000).");return false;}repeatCount=unsigned(std::stoul(value));}}return true;}
 void Play(){if(recording)FinishRecording();Stop();if(!Load())return;std::string error;if(!ParseMacro(script,steps,error)){SetStatus(error);return;}if(steps.empty()){SetStatus(English()?"Macro is empty.":Utf8(L"Macro đang trống."));return;}cursor=0;completedLoops=0;next=GetTickCount64();playing=true;SetStatus(English()?"Playing macro...":Utf8(L"Đang phát macro..."));}
 void Append(const std::string& action){auto now=GetTickCount64();if(now-started>600000||script.size()>4000000||recordedActions>=24000){FinishRecording();return;}if(now>last)script+="WAIT "+std::to_string(now-last)+"\r\n";script+=action+"\r\n";++recordedActions;last=now;}
 void Capture(const MSG& msg,bool allowed){if(!recording)return;if(!allowed||foreground()!=game){FinishRecording();return;}
  auto m=msg.message;auto key=unsigned(msg.wParam);
  if(m==WM_KEYDOWN||m==WM_SYSKEYDOWN||m==WM_KEYUP||m==WM_SYSKEYUP){if(m==WM_KEYDOWN||m==WM_SYSKEYDOWN){if(msg.lParam&(1L<<30))return;}if(key>=8&&key<=254){if(m==WM_KEYUP||m==WM_SYSKEYUP)recordedHeld.erase(int(key));else recordedHeld.insert(int(key));Append(std::string(m==WM_KEYUP||m==WM_SYSKEYUP?"KEYUP ":"KEYDOWN ")+std::to_string(key));}return;}
  if(m!=WM_MOUSEMOVE&&m!=WM_LBUTTONDOWN&&m!=WM_LBUTTONUP&&m!=WM_RBUTTONDOWN&&m!=WM_RBUTTONUP&&m!=WM_MBUTTONDOWN&&m!=WM_MBUTTONUP&&m!=WM_MOUSEWHEEL)return;
  if(m==WM_MOUSEMOVE&&GetTickCount64()-lastMove<20)return;
  RECT r{};GetClientRect(game,&r);POINT p{short(LOWORD(msg.lParam)),short(HIWORD(msg.lParam))};if(m==WM_MOUSEWHEEL)ScreenToClient(game,&p);if(r.right<=0||r.bottom<=0||p.x<0||p.y<0||p.x>=r.right||p.y>=r.bottom)return;
  Append("MOVE "+std::to_string(p.x*10000/r.right)+" "+std::to_string(p.y*10000/r.bottom));lastMove=GetTickCount64();
  if(m==WM_MOUSEWHEEL)Append("WHEEL "+std::to_string(short(HIWORD(msg.wParam))));
  else if(m!=WM_MOUSEMOVE){unsigned n=(m==WM_LBUTTONDOWN||m==WM_LBUTTONUP)?0:(m==WM_RBUTTONDOWN||m==WM_RBUTTONUP)?1:2;if(m==WM_LBUTTONDOWN||m==WM_RBUTTONDOWN||m==WM_MBUTTONDOWN)recordedMouse|=1u<<n;else recordedMouse&=~(1u<<n);Append(m==WM_LBUTTONDOWN?"LDOWN":m==WM_LBUTTONUP?"LUP":m==WM_RBUTTONDOWN?"RDOWN":m==WM_RBUTTONUP?"RUP":m==WM_MBUTTONDOWN?"MDOWN":"MUP");}
 }
 void Tick(bool allowed){if(!playing&&!recording)return;if(!allowed||foreground()!=game){if(recording)FinishRecording();Stop();SetStatus(English()?"Stopped: game is not active.":Utf8(L"Đã dừng: game không còn hoạt động."));return;}if(!playing)return;
  auto now=GetTickCount64();unsigned batch=0;while(playing&&now>=next&&cursor<steps.size()&&batch++<32){auto s=steps[cursor++];if(s.op=="WAIT"){next+=s.a;continue;}INPUT i{};
   if(s.op=="KEYDOWN"||s.op=="KEYUP"){i.type=INPUT_KEYBOARD;i.ki.wVk=WORD(s.a);if(s.op=="KEYUP"){i.ki.dwFlags=KEYEVENTF_KEYUP;held.erase(s.a);}else held.insert(s.a);}
   else {i.type=INPUT_MOUSE;
    if(s.op=="MOVE"){RECT r{};GetClientRect(game,&r);POINT p{s.a*std::max(0L,r.right-1)/10000,s.b*std::max(0L,r.bottom-1)/10000};ClientToScreen(game,&p);auto w=GetSystemMetrics(SM_CXVIRTUALSCREEN),h=GetSystemMetrics(SM_CYVIRTUALSCREEN);if(w<2||h<2){Stop();return;}i.mi.dx=(p.x-GetSystemMetrics(SM_XVIRTUALSCREEN))*65535/(w-1);i.mi.dy=(p.y-GetSystemMetrics(SM_YVIRTUALSCREEN))*65535/(h-1);i.mi.dwFlags=MOUSEEVENTF_MOVE|MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_VIRTUALDESK;}
    else if(s.op=="WHEEL"){i.mi.dwFlags=MOUSEEVENTF_WHEEL;i.mi.mouseData=DWORD(s.a);}
    else {bool down=s.op.find("DOWN")!=std::string::npos;unsigned n=s.op[0]=='L'?0:s.op[0]=='R'?1:2;i.mi.dwFlags=n==0?(down?MOUSEEVENTF_LEFTDOWN:MOUSEEVENTF_LEFTUP):n==1?(down?MOUSEEVENTF_RIGHTDOWN:MOUSEEVENTF_RIGHTUP):(down?MOUSEEVENTF_MIDDLEDOWN:MOUSEEVENTF_MIDDLEUP);if(down)mouseHeld|=1u<<n;else mouseHeld&=~(1u<<n);}}
   if(input(1,&i,sizeof(i))!=1){Stop();SetStatus(English()?"Input playback failed.":Utf8(L"Không phát được thao tác."));return;}
  }
  if(cursor>=steps.size()&&now>=next){if(NextLoop(now))return;SetStatus(English()?"Playback complete.":Utf8(L"Đã phát xong macro."));}
 }

 bool NextLoop(ULONGLONG now){++completedLoops;Stop();if(!repeatCount||completedLoops<repeatCount){cursor=0;next=now+10;playing=true;return true;}return false;}
 static std::string StepText(const MacroStep& s){return s.op+(s.op=="MOVE"?" "+std::to_string(s.a)+" "+std::to_string(s.b):s.op=="WAIT"||s.op=="WHEEL"||s.op=="KEYDOWN"||s.op=="KEYUP"?" "+std::to_string(s.a):"");}
 int Selected()const{return int(SendMessageW(grid,LVM_GETNEXTITEM,WPARAM(-1),LVNI_SELECTED));}
 void RefreshGrid(int select=-1){SendMessageW(grid,LVM_DELETEALLITEMS,0,0);for(size_t n=0;n<editorSteps.size();++n){const auto& s=editorSteps[n];auto index=std::to_wstring(n+1);LVITEMW item{};item.mask=LVIF_TEXT;item.iItem=int(n);item.pszText=index.data();SendMessageW(grid,LVM_INSERTITEMW,0,LPARAM(&item));std::wstring values[]={s.op=="WAIT"?(English()?L"Time":L"Th\u1eddi gian"):s.op.rfind("KEY",0)==0?(English()?L"Keyboard":L"B\u00e0n ph\u00edm"):(English()?L"Mouse":L"Chu\u1ed9t"),Wide(s.op),s.op=="MOVE"||s.op=="WAIT"||s.op=="WHEEL"||s.op.rfind("KEY",0)==0?std::to_wstring(s.a):L"",s.op=="MOVE"?std::to_wstring(s.b):L""};for(int col=0;col<4;++col){item.iSubItem=col+1;item.pszText=values[col].data();SendMessageW(grid,LVM_SETITEMTEXTW,n,LPARAM(&item));}}
  if(select>=0&&select<int(editorSteps.size())){LVITEMW item{};item.state=item.stateMask=LVIS_SELECTED|LVIS_FOCUSED;SendMessageW(grid,LVM_SETITEMSTATE,select,LPARAM(&item));SendMessageW(grid,LVM_ENSUREVISIBLE,select,FALSE);}}
 void SelectRow(){int n=Selected();if(n<0||n>=int(editorSteps.size()))return;auto& s=editorSteps[n];Category(s.op=="WAIT"?4:s.op.rfind("KEY",0)==0?3:2);auto op=Wide(s.op);auto at=SendMessageW(operation,CB_FINDSTRINGEXACT,WPARAM(-1),LPARAM(op.c_str()));SendMessageW(operation,CB_SETCURSEL,at,0);SetWindowTextW(argA,std::to_wstring(s.a).c_str());SetWindowTextW(argB,std::to_wstring(s.b).c_str());if(s.op.rfind("KEY",0)==0)ChooseKey(unsigned(s.a));UpdateFields();}
 std::wstring Field(HWND control){wchar_t value[64]{};GetWindowTextW(control,value,64);return value;}
 bool ReadStep(MacroStep& step){auto op=Utf8(Field(operation));auto text=op;if(op=="MOVE")text+=" "+Utf8(Field(argA))+" "+Utf8(Field(argB));else if(op.rfind("KEY",0)==0){auto selected=SendMessageW(keyPicker,CB_GETCURSEL,0,0);if(selected==CB_ERR)return false;text+=" "+std::to_string(SendMessageW(keyPicker,CB_GETITEMDATA,selected,0));}else if(op=="WAIT"||op=="WHEEL")text+=" "+Utf8(Field(argA));std::vector<MacroStep> result;std::string error;if(!ParseMacro(text,result,error)||result.size()!=1){SetStatus(English()?"Invalid values. MOVE: X/Y 0..10000; keys: virtual key 8..254; time: milliseconds.":Utf8(L"Gi\u00e1 tr\u1ecb kh\u00f4ng h\u1ee3p l\u1ec7. MOVE: X/Y 0..10000; ph\u00edm: m\u00e3 8..254; th\u1eddi gian: mili gi\u00e2y."));return false;}step=result.front();return true;}
 bool SaveGrid(){auto value=Utf8(Field(repeatEdit));if(value.empty()||value.size()>7||value.find_first_not_of("0123456789")!=std::string::npos||std::stoul(value)>1000000){SetStatus("Repeat: 0..1000000.");return false;}auto count=unsigned(std::stoul(value));std::string text="# REPEAT "+std::to_string(count)+"\r\n";for(auto& step:editorSteps)text+=StepText(step)+"\r\n";std::vector<MacroStep> validated;std::string error;if(!ParseMacro(text,validated,error)){SetStatus(error);return false;}script=text;repeatCount=count;if(!Save())return false;SetStatus(English()?"Saved. Ctrl+Shift+F10 runs; Ctrl+Shift+F12 stops.":Utf8(L"\u0110\u00e3 l\u01b0u. Ctrl+Shift+F10 ch\u1ea1y; Ctrl+Shift+F12 d\u1eebng."));return true;}


 static std::wstring KeyLabel(unsigned vk){
  if((vk>='A'&&vk<='Z')||(vk>='0'&&vk<='9'))return std::wstring(1,wchar_t(vk));
  if(vk>=VK_F1&&vk<=VK_F24)return L"F"+std::to_wstring(vk-VK_F1+1);
  if(vk>=VK_NUMPAD0&&vk<=VK_NUMPAD9)return L"Numpad "+std::to_wstring(vk-VK_NUMPAD0);
  switch(vk){case VK_BACK:return L"Backspace";case VK_TAB:return L"Tab";case VK_RETURN:return L"Enter";case VK_SHIFT:return L"Shift";case VK_CONTROL:return L"Ctrl";case VK_MENU:return L"Alt";case VK_PAUSE:return L"Pause";case VK_CAPITAL:return L"Caps Lock";case VK_ESCAPE:return L"Esc";case VK_SPACE:return L"Space";case VK_PRIOR:return L"Page Up";case VK_NEXT:return L"Page Down";case VK_END:return L"End";case VK_HOME:return L"Home";case VK_LEFT:return L"Left Arrow";case VK_UP:return L"Up Arrow";case VK_RIGHT:return L"Right Arrow";case VK_DOWN:return L"Down Arrow";case VK_SNAPSHOT:return L"Print Screen";case VK_INSERT:return L"Insert";case VK_DELETE:return L"Delete";case VK_LWIN:return L"Left Windows";case VK_RWIN:return L"Right Windows";case VK_APPS:return L"Menu";case VK_MULTIPLY:return L"Numpad *";case VK_ADD:return L"Numpad +";case VK_SUBTRACT:return L"Numpad -";case VK_DECIMAL:return L"Numpad .";case VK_DIVIDE:return L"Numpad /";case VK_NUMLOCK:return L"Num Lock";case VK_SCROLL:return L"Scroll Lock";case VK_LSHIFT:return L"Left Shift";case VK_RSHIFT:return L"Right Shift";case VK_LCONTROL:return L"Left Ctrl";case VK_RCONTROL:return L"Right Ctrl";case VK_LMENU:return L"Left Alt";case VK_RMENU:return L"Right Alt";
  case VK_OEM_1:return L"; / :";case VK_OEM_PLUS:return L"= / +";case VK_OEM_COMMA:return L", / <";case VK_OEM_MINUS:return L"- / _";case VK_OEM_PERIOD:return L". / >";case VK_OEM_2:return L"/ / ?";case VK_OEM_3:return L"` / ~";case VK_OEM_4:return L"[ / {";case VK_OEM_5:return L"Backslash / |";case VK_OEM_6:return L"] / }";case VK_OEM_7:return L"Quote";default:return {};}
 }
 void ChooseKey(unsigned vk){for(int i=0;i<int(SendMessageW(keyPicker,CB_GETCOUNT,0,0));++i)if(SendMessageW(keyPicker,CB_GETITEMDATA,i,0)==vk){SendMessageW(keyPicker,CB_SETCURSEL,i,0);return;}auto name=(English()?L"Recorded key ":L"Ph\u00edm \u0111\u00e3 ghi ")+std::to_wstring(vk);auto index=SendMessageW(keyPicker,CB_ADDSTRING,0,LPARAM(name.c_str()));SendMessageW(keyPicker,CB_SETITEMDATA,index,vk);SendMessageW(keyPicker,CB_SETCURSEL,index,0);}
 void UpdateFields(){auto op=Utf8(Field(operation));bool key=op.rfind("KEY",0)==0,move=op=="MOVE",number=move||op=="WAIT"||op=="WHEEL";
  MoveWindow(argA,move?190:150,50,move?65:186,24,TRUE);MoveWindow(labelA,move?190:150,79,move?65:186,18,TRUE);MoveWindow(labelB,279,79,57,18,TRUE);
  ShowWindow(keyPicker,key?SW_SHOW:SW_HIDE);ShowWindow(argA,number?SW_SHOW:SW_HIDE);ShowWindow(argB,move?SW_SHOW:SW_HIDE);ShowWindow(labelA,(number||key)?SW_SHOW:SW_HIDE);ShowWindow(labelB,move?SW_SHOW:SW_HIDE);
  SetWindowTextW(labelA,key?(English()?L"Key":L"Ph\u00edm"):move?L"X":op=="WAIT"?(English()?L"Milliseconds":L"Mili gi\u00e2y"):(English()?L"Wheel delta":L"Cu\u1ed9n"));SetWindowTextW(labelB,L"Y");
 }
 void Category(int id){SendMessageW(operation,CB_RESETCONTENT,0,0);const char* mouse[]={"MOVE","LDOWN","LUP","RDOWN","RUP","MDOWN","MUP","WHEEL"};const char* keyboard[]={"KEYDOWN","KEYUP"};if(id==2)for(auto op:mouse){auto wide=Wide(op);SendMessageW(operation,CB_ADDSTRING,0,LPARAM(wide.c_str()));}else if(id==3)for(auto op:keyboard){auto wide=Wide(op);SendMessageW(operation,CB_ADDSTRING,0,LPARAM(wide.c_str()));}else SendMessageW(operation,CB_ADDSTRING,0,LPARAM(L"WAIT"));SendMessageW(operation,CB_SETCURSEL,0,0);SetWindowTextW(argA,id==3?L"81":id==4?L"1000":L"5000");SetWindowTextW(argB,L"5000");if(id==3)ChooseKey(81);UpdateFields();}
 static LRESULT CALLBACK EditorProc(HWND w,UINT m,WPARAM wp,LPARAM lp){auto self=reinterpret_cast<MacroRecorder*>(GetWindowLongPtrW(w,GWLP_USERDATA));if(m==WM_NCCREATE){self=static_cast<MacroRecorder*>(reinterpret_cast<CREATESTRUCTW*>(lp)->lpCreateParams);SetWindowLongPtrW(w,GWLP_USERDATA,reinterpret_cast<LONG_PTR>(self));}
  if(self&&m==WM_COMMAND){auto id=LOWORD(wp);if(id==11&&HIWORD(wp)==CBN_SELCHANGE){self->UpdateFields();return 0;}if(id==1){self->SaveGrid();return 0;}if(id>=2&&id<=4){self->Category(id);return 0;}int n=self->Selected();if(id==5||id==6){MacroStep step;if(!self->ReadStep(step))return 0;if(id==6){if(n<0)return 0;self->editorSteps[n]=step;}else{if(self->editorSteps.size()>=50000)return 0;n=n<0?int(self->editorSteps.size()):n+1;self->editorSteps.insert(self->editorSteps.begin()+n,step);}self->RefreshGrid(n);return 0;}if(id==7&&n>=0){self->editorSteps.erase(self->editorSteps.begin()+n);self->RefreshGrid(std::min(n,int(self->editorSteps.size())-1));return 0;}if((id==8||id==9)&&n>=0){int target=n+(id==8?-1:1);if(target>=0&&target<int(self->editorSteps.size())){std::swap(self->editorSteps[n],self->editorSteps[target]);self->RefreshGrid(target);}return 0;}}
  if(self&&m==WM_NOTIFY){auto hdr=reinterpret_cast<NMHDR*>(lp);if(hdr->hwndFrom==self->grid&&hdr->code==LVN_ITEMCHANGED){self->SelectRow();return 0;}}
  if(self&&m==WM_SIZE&&self->grid){int width=LOWORD(lp),height=HIWORD(lp);MoveWindow(self->grid,12,128,std::max(100,width-24),std::max(80,height-184),TRUE);MoveWindow(self->statusLabel,12,std::max(210,height-48),std::max(100,width-24),40,TRUE);return 0;}
  if(self&&m==WM_DESTROY){self->editor=self->grid=self->operation=self->argA=self->argB=self->repeatEdit=self->keyPicker=self->labelA=self->labelB=self->statusLabel=nullptr;return 0;}return DefWindowProcW(w,m,wp,lp);
 }
 void Edit(){if(recording)FinishRecording();Stop();if(editor){SetForegroundWindow(editor);return;}Load();std::string error;if(!ParseMacro(script,editorSteps,error)){SetStatus(error);return;}
  auto library=LoadLibraryW(L"comctl32.dll");if(!library)return;using InitFn=BOOL(WINAPI*)(const INITCOMMONCONTROLSEX*);auto init=reinterpret_cast<InitFn>(GetProcAddress(library,"InitCommonControlsEx"));INITCOMMONCONTROLSEX cc{sizeof(cc),ICC_LISTVIEW_CLASSES};if(!init||!init(&cc))return;
  WNDCLASSW c{};c.lpfnWndProc=EditorProc;c.hInstance=GetModuleHandleW(nullptr);c.lpszClassName=L"WHTMacroEditor";c.hCursor=LoadCursor(nullptr,IDC_ARROW);c.hbrBackground=HBRUSH(COLOR_WINDOW+1);RegisterClassW(&c);
  editor=CreateWindowW(c.lpszClassName,English()?L"WHT - Macro Script":L"WHT - Ch\u1ec9nh s\u1eeda macro",WS_OVERLAPPEDWINDOW,CW_USEDEFAULT,CW_USEDEFAULT,840,620,nullptr,nullptr,c.hInstance,this);if(!editor)return;
  auto control=[&](const wchar_t* cls,const wchar_t* text,DWORD style,int x,int y,int width,int height,int id){auto child=CreateWindowW(cls,text,WS_CHILD|WS_VISIBLE|style,x,y,width,height,editor,HMENU(INT_PTR(id)),c.hInstance,nullptr);SendMessageW(child,WM_SETFONT,WPARAM(GetStockObject(DEFAULT_GUI_FONT)),TRUE);return child;};
  control(L"BUTTON",English()?L"Mouse":L"Chu\u1ed9t",0,12,12,90,28,2);control(L"BUTTON",English()?L"Keyboard":L"B\u00e0n ph\u00edm",0,108,12,100,28,3);control(L"BUTTON",English()?L"Time":L"Th\u1eddi gian",0,214,12,100,28,4);control(L"BUTTON",English()?L"Save":L"L\u01b0u",0,320,12,80,28,1);
  control(L"STATIC",English()?L"Repeat (0 = infinite)":L"S\u1ed1 l\u1ea7n l\u1eb7p (0 = v\u00f4 h\u1ea1n)",0,416,18,210,24,0);repeatEdit=control(L"EDIT",std::to_wstring(repeatCount).c_str(),WS_BORDER|ES_NUMBER|ES_AUTOHSCROLL,632,14,95,24,10);
  operation=control(L"COMBOBOX",L"",CBS_DROPDOWNLIST|WS_VSCROLL,12,50,132,220,11);argA=control(L"EDIT",L"5000",WS_BORDER|ES_AUTOHSCROLL,190,50,65,24,12);argB=control(L"EDIT",L"5000",WS_BORDER|ES_AUTOHSCROLL,279,50,57,24,13);labelA=control(L"STATIC",L"X",0,150,30,175,18,0);labelB=control(L"STATIC",L"Y",0,262,30,40,18,0);MoveWindow(labelA,150,79,175,18,TRUE);MoveWindow(labelB,279,79,40,18,TRUE);keyPicker=control(L"COMBOBOX",L"",CBS_DROPDOWNLIST|WS_VSCROLL,150,50,186,280,14);for(unsigned vk=8;vk<=254;++vk){auto name=KeyLabel(vk);if(name.empty())continue;auto index=SendMessageW(keyPicker,CB_ADDSTRING,0,LPARAM(name.c_str()));SendMessageW(keyPicker,CB_SETITEMDATA,index,vk);if(vk==81)SendMessageW(keyPicker,CB_SETCURSEL,index,0);}
  control(L"BUTTON",English()?L"Add":L"Th\u00eam",0,350,48,76,28,5);control(L"BUTTON",English()?L"Apply row":L"S\u1eeda d\u00f2ng",0,432,48,98,28,6);control(L"BUTTON",English()?L"Delete":L"X\u00f3a",0,536,48,76,28,7);control(L"BUTTON",English()?L"Up":L"L\u00ean",0,618,48,65,28,8);control(L"BUTTON",English()?L"Down":L"Xu\u1ed1ng",0,689,48,70,28,9);
  control(L"STATIC",English()?L"MOVE: X / Y (0..10000); KEY: virtual key (8..254); WAIT: ms; WHEEL: delta. Select a row to edit.":L"MOVE: X / Y (0..10000); KEY: m\u00e3 ph\u00edm (8..254); WAIT: mili gi\u00e2y; WHEEL: cu\u1ed9n. Ch\u1ecdn d\u00f2ng \u0111\u1ec3 s\u1eeda.",0,12,103,780,24,0);
  grid=control(WC_LISTVIEWW,L"",WS_BORDER|WS_TABSTOP|LVS_REPORT|LVS_SINGLESEL|LVS_SHOWSELALWAYS,12,128,800,390,20);SendMessageW(grid,LVM_SETEXTENDEDLISTVIEWSTYLE,0,LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES|LVS_EX_DOUBLEBUFFER);
  const wchar_t* titles[]={L"#",English()?L"Type":L"Lo\u1ea1i",English()?L"Action":L"Thao t\u00e1c",English()?L"X / Key / ms":L"X / Ph\u00edm / ms",L"Y"};int widths[]={55,150,170,180,150};for(int n=0;n<5;++n){LVCOLUMNW col{};col.mask=LVCF_TEXT|LVCF_WIDTH;col.pszText=const_cast<wchar_t*>(titles[n]);col.cx=widths[n];SendMessageW(grid,LVM_INSERTCOLUMNW,n,LPARAM(&col));}
  statusLabel=control(L"STATIC",L"",0,12,528,800,40,0);Category(2);RefreshGrid();SetStatus(English()?"Select a row, edit its values, then Apply row and Save.":Utf8(L"Ch\u1ecdn d\u00f2ng, s\u1eeda gi\u00e1 tr\u1ecb, b\u1ea5m S\u1eeda d\u00f2ng r\u1ed3i L\u01b0u."));ShowWindow(editor,SW_SHOW);SetForegroundWindow(editor);
 }
};
}
