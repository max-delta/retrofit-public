#pragma once
#include "core/compiler.h"

#include "rftl/string_view"
#include "rftl/memory"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits = rftl::char_traits<CharT>>
class immutable_basic_string
{
	//
	// Types
public:
	using backing_type = scoped_ptr<CharT const>;
	using view_type = basic_string_view<CharT, Traits>;
	using traits_type = typename view_type::traits_type;
	using value_type = typename view_type::value_type;
	using size_type = typename view_type::size_type;
	using difference_type = typename view_type::difference_type;
	using const_reference = typename view_type::const_reference;
	using const_pointer = typename view_type::const_pointer;
	using const_iterator = typename view_type::const_iterator;
	using const_reverse_iterator = typename view_type::const_reverse_iterator;


	//
	// Public methods (conversion)
public:
	immutable_basic_string() = default;
	immutable_basic_string( view_type value )
		: mStorage( create( value ) )
		, mSize( value.size() )
	{
		//
	}
	immutable_basic_string( CharT const* value )
		: immutable_basic_string( view_type( value ) )
	{
		//
	}
	template<typename Allocator>
	immutable_basic_string( basic_string<CharT, Traits, Allocator> const& value )
		: immutable_basic_string( view_type( value ) )
	{
		//
	}
	immutable_basic_string( immutable_basic_string const& value )
		: immutable_basic_string( view_type( value ) )
	{
		//
	}
	immutable_basic_string& operator=( immutable_basic_string const& ) = delete;

	operator view_type() const
	{
		return str();
	}

	view_type str() const
	{
		return view_type( mStorage.get(), mSize );
	}

	char const* data() const
	{
		return mStorage.get();
	}

	basic_string<CharT, Traits> clone() const
	{
		return basic_string<CharT, Traits>( mStorage.get(), mSize );
	}



	//
	// Public methods (passthrough)
public:
	bool empty() const
	{
		return mSize == 0;
	}


	//
	// Private methods
private:
	static backing_type create( view_type value )
	{
		size_type const size = value.size();
		size_type const sizeWithNull = size + 1;
		value_type* ptr = new value_type[sizeWithNull]();
		memcpy( ptr, value.data(), size );
		ptr[size] = {};
		return backing_type( ptr );
	}

	//
	// Private data
private:
	backing_type const mStorage = {};
	size_type const mSize = 0;
};

using immutable_string = immutable_basic_string<char>;
using immutable_wstring = immutable_basic_string<wchar_t>;
using immutable_u8string = immutable_basic_string<char8_t>;
using immutable_u16string = immutable_basic_string<char16_t>;
using immutable_u32string = immutable_basic_string<char32_t>;

///////////////////////////////////////////////////////////////////////////////

RF_CLANG_PUSH();
RF_CLANG_IGNORE( "-Wmicrosoft-template" );
template<typename CharT, typename Traits>
struct hash<immutable_basic_string<CharT, Traits>>
{
	// HACK: MSVC requires 'rftl::' in the param here, for unknown reasons
	size_t operator()( rftl::immutable_basic_string<CharT, Traits> const& string ) const
	{
		return hash<basic_string_view<CharT, Traits>>()( string );
	}
};
RF_CLANG_POP();

template<typename CharT, typename Traits = rftl::char_traits<CharT>>
bool operator==(
	immutable_basic_string<CharT, Traits> const& lhs,
	immutable_basic_string<CharT, Traits> const& rhs )
{
	return static_cast<basic_string_view<CharT, Traits> const&>( lhs ) ==
		static_cast<basic_string_view<CharT, Traits> const&>( rhs );
}



template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool operator==(
	immutable_basic_string<CharT, Traits> const& lhs,
	basic_string<CharT, Traits, Allocator> const& rhs )
{
	return static_cast<basic_string_view<CharT, Traits> const&>( lhs ) ==
		static_cast<basic_string_view<CharT, Traits> const&>( rhs );
}



// TODO: This duplicate shouldn't be needed when C++20 is available
template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool operator==(
	basic_string<CharT, Traits, Allocator> const& lhs,
	immutable_basic_string<CharT, Traits> const& rhs )
{
	return static_cast<basic_string_view<CharT, Traits> const&>( lhs ) ==
		static_cast<basic_string_view<CharT, Traits> const&>( rhs );
}

///////////////////////////////////////////////////////////////////////////////
}
