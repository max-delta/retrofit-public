#pragma once
#include "rftl/sstream"
#include "rftl/string"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename... ArgsT>
basic_string<CharT> basic_ss_concat( ArgsT... args );

template<typename... ArgsT>
string ss_concat( ArgsT... args );
template<typename... ArgsT>
wstring wss_concat( ArgsT... args );
template<typename... ArgsT>
u8string u8ss_concat( ArgsT... args );
template<typename... ArgsT>
u16string u16ss_concat( ArgsT... args );
template<typename... ArgsT>
u32string u32ss_concat( ArgsT... args );

///////////////////////////////////////////////////////////////////////////////
}

#include "ss_concat.inl"
