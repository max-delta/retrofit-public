#pragma once
#include "core/compiler.h"

#include "rftl/string"
#include "rftl/memory"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
class immutable_basic_string
{
	//
	// Types
public:
	using backing_type = basic_string<CharT, Traits, Allocator>;
	using traits_type = typename backing_type::traits_type;
	using value_type = typename backing_type::value_type;
	using allocator_type = typename backing_type::allocator_type;
	using size_type = typename backing_type::size_type;
	using difference_type = typename backing_type::difference_type;
	using const_reference = typename backing_type::const_reference;
	using const_pointer = typename backing_type::const_pointer;
	using const_iterator = typename backing_type::const_iterator;
	using const_reverse_iterator = typename backing_type::const_reverse_iterator;


	//
	// Public methods (conversion)
public:
	immutable_basic_string() = default;
	immutable_basic_string( backing_type const& value )
		: mStorage( value )
	{
		//
	}
	immutable_basic_string( CharT const* value )
		: immutable_basic_string( backing_type( value ) )
	{
		//
	}
	immutable_basic_string& operator=( immutable_basic_string const& ) = delete;

	operator backing_type const&() const
	{
		return mStorage;
	}

	backing_type const& str() const
	{
		return mStorage;
	}


	//
	// Public methods (passthrough)
public:
	bool empty() const
	{
		return mStorage.empty();
	}


	//
	// Private data
private:
	backing_type const mStorage;
};

using immutable_string = immutable_basic_string<char>;
using immutable_wstring = immutable_basic_string<wchar_t>;
using immutable_u16string = immutable_basic_string<char16_t>;
using immutable_u32string = immutable_basic_string<char32_t>;

///////////////////////////////////////////////////////////////////////////////

RF_CLANG_PUSH();
RF_CLANG_IGNORE( "-Wmicrosoft-template" );
template<typename CharT, typename Traits, typename Allocator>
struct hash<immutable_basic_string<CharT, Traits, Allocator>>
{
	// HACK: MSVC requires 'rftl::' in the param here, for unknown reasons
	size_t operator()( rftl::immutable_basic_string<CharT, Traits, Allocator> const& string ) const
	{
		return hash<basic_string<CharT, Traits, Allocator>>()( string );
	}
};
RF_CLANG_POP();

template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool operator==(
	immutable_basic_string<CharT, Traits, Allocator> const& lhs,
	immutable_basic_string<CharT, Traits, Allocator> const& rhs )
{
	return static_cast<basic_string<CharT, Traits, Allocator> const&>( lhs ) ==
		static_cast<basic_string<CharT, Traits, Allocator> const&>( rhs );
}



template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool operator==(
	immutable_basic_string<CharT, Traits, Allocator> const& lhs,
	basic_string<CharT, Traits, Allocator> const& rhs )
{
	return static_cast<basic_string<CharT, Traits, Allocator> const&>( lhs ) ==
		static_cast<basic_string<CharT, Traits, Allocator> const&>( rhs );
}



// TODO: This duplicate shouldn't be needed when C++20 is available
template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool operator==(
	basic_string<CharT, Traits, Allocator> const& lhs,
	immutable_basic_string<CharT, Traits, Allocator> const& rhs )
{
	return static_cast<basic_string<CharT, Traits, Allocator> const&>( lhs ) ==
		static_cast<basic_string<CharT, Traits, Allocator> const&>( rhs );
}

///////////////////////////////////////////////////////////////////////////////
}
