#pragma once


namespace RF::term::ansi {
///////////////////////////////////////////////////////////////////////////////

// \x1b - Escape
// [ - Control sequence introducer
// m - Select graphic rendition parameters
namespace csi::sgr {
inline constexpr char kReset[] = "\x1b[0m";

inline constexpr char kBlack[] = "\x1b[30m";
inline constexpr char kRed[] = "\x1b[31m";
inline constexpr char kGreen[] = "\x1b[32m";
inline constexpr char kYellow[] = "\x1b[33m";
inline constexpr char kBlue[] = "\x1b[34m";
inline constexpr char kMagenta[] = "\x1b[35m";
inline constexpr char kCyan[] = "\x1b[36m";
inline constexpr char kWhite[] = "\x1b[37m";

inline constexpr char kBrightBlack[] = "\x1b[1;30m";
inline constexpr char kBrightRed[] = "\x1b[1;31m";
inline constexpr char kBrightGreen[] = "\x1b[1;32m";
inline constexpr char kBrightYellow[] = "\x1b[1;33m";
inline constexpr char kBrightBlue[] = "\x1b[1;34m";
inline constexpr char kBrightMagenta[] = "\x1b[1;35m";
inline constexpr char kBrightCyan[] = "\x1b[1;36m";
inline constexpr char kBrightWhite[] = "\x1b[1;37m";
}

///////////////////////////////////////////////////////////////////////////////
}
