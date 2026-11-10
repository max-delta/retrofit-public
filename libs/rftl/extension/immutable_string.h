#pragma once
#include "rftl/extension/simple_string.h"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// A stricter version of simple string, that doesn't allow even the structure
//  itself to be modified once constructed
template<typename CharT, typename Traits = rftl::char_traits<CharT>>
class basic_immutable_string : private basic_simple_string<CharT, Traits>
{
	RF_NO_ASSIGN_VIA_COPY( basic_immutable_string );

	//
	// Types
public:
	using basic_simple_string = basic_simple_string<CharT, Traits>;
	using basic_simple_string::view_type;


	//
	// Public methods
public:
	using basic_simple_string::basic_simple_string;
	using basic_simple_string::operator view_type;
	using basic_simple_string::view;
	using basic_simple_string::c_str;
	using basic_simple_string::empty;
};

using immutable_string = basic_immutable_string<char>;
using immutable_wstring = basic_immutable_string<wchar_t>;
using immutable_u8string = basic_immutable_string<char8_t>;
using immutable_u16string = basic_immutable_string<char16_t>;
using immutable_u32string = basic_immutable_string<char32_t>;

///////////////////////////////////////////////////////////////////////////////

RF_CLANG_PUSH();
RF_CLANG_IGNORE( "-Wmicrosoft-template" );
template<typename CharT, typename Traits>
struct hash<basic_immutable_string<CharT, Traits>>
{
	// HACK: MSVC requires 'rftl::' in the param here, for unknown reasons
	size_t operator()( rftl::basic_immutable_string<CharT, Traits> const& string ) const
	{
		return hash<basic_string_view<CharT, Traits>>()( string );
	}
};
RF_CLANG_POP();

template<typename CharT, typename Traits = rftl::char_traits<CharT>>
bool operator==(
	basic_immutable_string<CharT, Traits> const& lhs,
	basic_immutable_string<CharT, Traits> const& rhs )
{
	return static_cast<basic_string_view<CharT, Traits> const&>( lhs ) ==
		static_cast<basic_string_view<CharT, Traits> const&>( rhs );
}



template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool operator==(
	basic_immutable_string<CharT, Traits> const& lhs,
	basic_string<CharT, Traits, Allocator> const& rhs )
{
	return static_cast<basic_string_view<CharT, Traits> const&>( lhs ) ==
		static_cast<basic_string_view<CharT, Traits> const&>( rhs );
}



// TODO: This duplicate shouldn't be needed when C++20 is available
template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool operator==(
	basic_string<CharT, Traits, Allocator> const& lhs,
	basic_immutable_string<CharT, Traits> const& rhs )
{
	return static_cast<basic_string_view<CharT, Traits> const&>( lhs ) ==
		static_cast<basic_string_view<CharT, Traits> const&>( rhs );
}

///////////////////////////////////////////////////////////////////////////////
}
