#pragma once
#include <array>
namespace wht {
// Owned feature frames retain their requested visibility while their parent
// handles engine clipping. Only state transitions need native registration.
struct MthPresentation131 {
 bool visibilityKnown=false,visible=false,layoutKnown=false;
 std::array<float,4> bounds{};
 template<class Show,class Hide> void Visibility(bool requested,Show show,Hide hide){
  if(visibilityKnown&&visible==requested)return;
  visibilityKnown=true;visible=requested;if(requested)show();else hide();
 }
 template<class Place> void Layout(float left,float bottom,float right,float top,bool textChanged,Place place){
  const std::array<float,4> next{left,bottom,right,top};
  if(layoutKnown&&bounds==next&&!textChanged)return;
  bounds=next;layoutKnown=true;place();
 }
};
}
