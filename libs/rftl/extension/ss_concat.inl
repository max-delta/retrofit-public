#pragma once
#include "ss_concat.h"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename... ArgsT>
inline basic_string<CharT> basic_ss_concat( ArgsT... args )
{
	basic_stringstream<CharT> ss;
	( ( ss << args ), ... );
	return ss.rdbuf()->str();
}

template<typename... ArgsT>
inline string ss_concat( ArgsT... args )
{
	return basic_ss_concat<char>( args... );
}

template<typename... ArgsT>
inline wstring wss_concat( ArgsT... args )
{
	return basic_ss_concat<wchar_t>( args... );
}

template<typename... ArgsT>
inline u8string u8ss_concat( ArgsT... args )
{
	return basic_ss_concat<char8_t>( args... );
}

template<typename... ArgsT>
inline u16string u16ss_concat( ArgsT... args )
{
	return basic_ss_concat<char16_t>( args... );
}

template<typename... ArgsT>
inline u32string u32ss_concat( ArgsT... args )
{
	return basic_ss_concat<char32_t>( args... );
}

///////////////////////////////////////////////////////////////////////////////
}
