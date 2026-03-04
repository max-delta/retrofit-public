#pragma once
#include "rftl/initializer_list"
#include "rftl/string"
#include "rftl/string_view"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// Like a set<CharT>, but as a basic_string<...>, since that is a form used by
//  other STL algorithms
template<typename CharT>
class basic_unique_char_set : public basic_string<CharT>
{
	//
	// Types
public:
	using Base = basic_string<CharT>;
	using view_type = basic_string_view<typename Base::value_type>;


	//
	// Public methods
public:
	basic_unique_char_set() = default;
	basic_unique_char_set( initializer_list<typename Base::value_type> str );
	basic_unique_char_set( view_type str );

	size_t count( typename Base::value_type ch ) const;
};


using unique_char_set = basic_unique_char_set<char>;
using unique_wchar_set = basic_unique_char_set<wchar_t>;
using unique_u8char_set = basic_unique_char_set<char8_t>;
using unique_u16char_set = basic_unique_char_set<char16_t>;
using unique_u32char_set = basic_unique_char_set<char32_t>;

///////////////////////////////////////////////////////////////////////////////
}

#include "unique_char_set.inl"
