#pragma once
#include "core/compiler.h"

#include "rftl/type_traits"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// By default, assume non-char
template<typename CharT>
struct is_char : public rftl::false_type
{
};

// Treat const as unimportant
template<typename CharT>
struct is_char<CharT const> : public is_char<CharT>
{
};

// Treat ref as unimportant
template<typename CharT>
struct is_char<CharT&> : public is_char<CharT>
{
};

// Support native char types
template<>
struct is_char<char> : public rftl::true_type
{
};
template<>
struct is_char<wchar_t> : public rftl::true_type
{
};
template<>
struct is_char<char16_t> : public rftl::true_type
{
};
template<>
struct is_char<char32_t> : public rftl::true_type
{
};

///////////////////////////////////////////////////////////////////////////////

// By default, assume non-char-array
template<typename ArrayT, typename UnusedT = ArrayT>
struct is_char_array : public rftl::false_type
{
};

// Support arrays of chars
template<typename CharT>
struct is_char_array<CharT[], typename rftl::enable_if<rftl::is_char<CharT>::value, CharT>::type[]> : public rftl::true_type
{
};

// Support array references of chars
template<typename CharT>
struct is_char_array<CharT ( & )[], typename rftl::enable_if<rftl::is_char<CharT>::value, CharT>::type ( & )[]> : public rftl::true_type
{
};

// Support sized arrays of chars
template<typename CharT, size_t Len>
struct is_char_array<CharT[Len], typename rftl::enable_if<rftl::is_char<CharT>::value, CharT>::type[Len]> : public rftl::true_type
{
};

// Support sized array references of chars
template<typename CharT, size_t Len>
struct is_char_array<CharT ( & )[Len], typename rftl::enable_if<rftl::is_char<CharT>::value, CharT>::type ( & )[Len]> : public rftl::true_type
{
};

#ifdef RF_PLATFORM_MSVC
// As usual, MSVC can't figure anything out so we have to do its job for it
template<typename CharT, size_t Len>
struct is_char_array<CharT const[Len], CharT const[Len]> : public is_char_array<CharT[Len], CharT[Len]>
{
};
template<typename CharT, size_t Len>
struct is_char_array<CharT ( & )[Len], CharT ( & )[Len]> : public is_char_array<CharT[Len], CharT[Len]>
{
};
template<size_t Len>
struct is_char_array<char[Len], char[Len]> : public rftl::true_type
{
};
template<size_t Len>
struct is_char_array<wchar_t[Len], wchar_t[Len]> : public rftl::true_type
{
};
template<size_t Len>
struct is_char_array<char16_t[Len], char16_t[Len]> : public rftl::true_type
{
};
template<size_t Len>
struct is_char_array<char32_t[Len], char32_t[Len]> : public rftl::true_type
{
};
#endif

///////////////////////////////////////////////////////////////////////////////

// By default, assume non-sized-char-array
template<typename ArrayT, typename UnusedT = ArrayT>
struct is_sized_char_array : public rftl::false_type
{
};

// Support sized arrays of chars
template<typename CharT, size_t Len>
struct is_sized_char_array<CharT[Len], typename rftl::enable_if<rftl::is_char<CharT>::value, CharT>::type[Len]> : public rftl::true_type
{
};

// Support sized array references of chars
template<typename CharT, size_t Len>
struct is_sized_char_array<CharT ( & )[Len], typename rftl::enable_if<rftl::is_char<CharT>::value, CharT>::type ( & )[Len]> : public rftl::true_type
{
};

#ifdef RF_PLATFORM_MSVC
// As usual, MSVC can't figure anything out so we have to do its job for it
template<typename CharT, size_t Len>
struct is_sized_char_array<CharT const[Len], CharT const[Len]> : public is_sized_char_array<CharT[Len], CharT[Len]>
{
};
template<typename CharT, size_t Len>
struct is_sized_char_array<CharT ( & )[Len], CharT ( & )[Len]> : public is_sized_char_array<CharT[Len], CharT[Len]>
{
};
template<size_t Len>
struct is_sized_char_array<char[Len], char[Len]> : public rftl::true_type
{
};
template<size_t Len>
struct is_sized_char_array<wchar_t[Len], wchar_t[Len]> : public rftl::true_type
{
};
template<size_t Len>
struct is_sized_char_array<char16_t[Len], char16_t[Len]> : public rftl::true_type
{
};
template<size_t Len>
struct is_sized_char_array<char32_t[Len], char32_t[Len]> : public rftl::true_type
{
};
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
}
