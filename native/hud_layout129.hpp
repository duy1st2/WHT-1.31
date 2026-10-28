#pragma once
namespace wht {
inline constexpr float TimerCenterX129=.34f;
inline constexpr float TimerCenterY129=.568f;
inline constexpr float MemoryCenterY129=.551f;
inline constexpr float PingLeft129=.428f;
inline constexpr float PingBottom129=.548f;
inline constexpr float HudCenterX129(bool hideTimeOfDay){return hideTimeOfDay?.4f:TimerCenterX129;}
inline constexpr float FpsCenterX129=.4f;
inline constexpr float FpsCenterY129=.535f;
inline constexpr float TimerTextHeight129=.016f;
inline constexpr float MemoryTextHeight129=.015f;
inline constexpr float FpsTextHeight129=.011f;
}
