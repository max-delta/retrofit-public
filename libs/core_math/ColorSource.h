#pragma once
#include "core_math/Color3u8.h"


namespace RF::math::details::color_source {
///////////////////////////////////////////////////////////////////////////////

// These are used to initialize all the different color types
// NOTE: Done via constexpr so that it does not run afoul of CRT-init ordering
//  issues across multiple compilation units

inline constexpr Color3u8 kBlack{ 0, 0, 0 };
inline constexpr Color3u8 kGray25{ 64, 64, 64 };
inline constexpr Color3u8 kGray50{ 127, 127, 127 };
inline constexpr Color3u8 kGray75{ 192, 192, 192 };
inline constexpr Color3u8 kWhite{ 255, 255, 255 };

inline constexpr Color3u8 kRed{ 255, 0, 0 };
inline constexpr Color3u8 kGreen{ 0, 255, 0 };
inline constexpr Color3u8 kBlue{ 0, 0, 255 };

inline constexpr Color3u8 kCyan{ 0, 255, 255 };
inline constexpr Color3u8 kMagenta{ 255, 0, 255 };
inline constexpr Color3u8 kYellow{ 255, 255, 0 };

inline constexpr Color3u8 kTeal{ 0, 255, 127 };
inline constexpr Color3u8 kPurple{ 127, 0, 255 };
inline constexpr Color3u8 kOrange{ 255, 127, 0 };

///////////////////////////////////////////////////////////////////////////////
}
