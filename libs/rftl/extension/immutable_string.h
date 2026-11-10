#pragma once
#include "core/compiler.h"
#include "core/macros.h"

#include "rftl/string_view"
#include "rftl/memory"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

template<typename CharT, typename Traits = rftl::char_traits<CharT>>
class immutable_basic_string
{
	RF_NO_ASSIGN_VIA_COPY( immutable_basic_string );

	//
	// Types
public:
	using view_type = basic_string_view<CharT, Traits>;
private:
	using backing_type = scoped_ptr<CharT const>;


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

	operator view_type() const
	{
		return view();
	}

	view_type view() const
	{
		return view_type( mStorage.get(), mSize );
	}

	char const* c_str() const
	{
		return mStorage.get();
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
		size_t const size = value.size();
		size_t const sizeWithNull = size + 1;
		CharT* ptr = new CharT[sizeWithNull]();
		memcpy( ptr, value.data(), size );
		ptr[size] = {};
		return backing_type( ptr );
	}


	//
	// Private data
private:
	backing_type const mStorage = {};
	size_t const mSize = 0;
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
