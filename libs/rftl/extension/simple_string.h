#pragma once
#include "core/compiler.h"
#include "core/macros.h"

#include "rftl/string_view"
#include "rftl/memory"


namespace rftl {
///////////////////////////////////////////////////////////////////////////////

// String class where the character data is immutable, allowing the heap
//  allocation to have the capacity always match the size, and thus to have
//  less member storage in the class itself
template<typename CharT, typename Traits = rftl::char_traits<CharT>>
class basic_simple_string
{
	RF_NO_ASSIGN_VIA_COPY( basic_simple_string );

	//
	// Types
public:
	using view_type = basic_string_view<CharT, Traits>;
private:
	using backing_type = scoped_ptr<CharT const[]>;


	//
	// Public methods
public:
	basic_simple_string() = default;
	basic_simple_string( view_type value )
		: mStorage( create( value ) )
		, mSize( value.size() )
	{
		//
	}
	basic_simple_string( CharT const* value )
		: basic_simple_string( view_type( value ) )
	{
		//
	}
	template<typename Allocator>
	basic_simple_string( basic_string<CharT, Traits, Allocator> const& value )
		: basic_simple_string( view_type( value ) )
	{
		//
	}
	basic_simple_string( basic_simple_string const& value )
		: basic_simple_string( view_type( value ) )
	{
		//
	}

	basic_simple_string& operator=( basic_simple_string&& rhs )
	{
		RF_MOVE_ASSIGN_CLEAR( mStorage );
		RF_MOVE_ASSIGN_CLEAR( mSize );
		return *this;
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

	bool empty() const
	{
		return mSize == 0;
	}

	void clear()
	{
		*this = {};
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
	backing_type mStorage = {};
	size_t mSize = 0;
};

using simple_string = basic_simple_string<char>;
using simple_wstring = basic_simple_string<wchar_t>;
using simple_u8string = basic_simple_string<char8_t>;
using simple_u16string = basic_simple_string<char16_t>;
using simple_u32string = basic_simple_string<char32_t>;

///////////////////////////////////////////////////////////////////////////////

RF_CLANG_PUSH();
RF_CLANG_IGNORE( "-Wmicrosoft-template" );
template<typename CharT, typename Traits>
struct hash<basic_simple_string<CharT, Traits>>
{
	// HACK: MSVC requires 'rftl::' in the param here, for unknown reasons
	size_t operator()( rftl::basic_simple_string<CharT, Traits> const& string ) const
	{
		return hash<basic_string_view<CharT, Traits>>()( string );
	}
};
RF_CLANG_POP();

template<typename CharT, typename Traits = rftl::char_traits<CharT>>
bool operator==(
	basic_simple_string<CharT, Traits> const& lhs,
	basic_simple_string<CharT, Traits> const& rhs )
{
	return static_cast<basic_string_view<CharT, Traits> const&>( lhs ) ==
		static_cast<basic_string_view<CharT, Traits> const&>( rhs );
}



template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool operator==(
	basic_simple_string<CharT, Traits> const& lhs,
	basic_string<CharT, Traits, Allocator> const& rhs )
{
	return static_cast<basic_string_view<CharT, Traits> const&>( lhs ) ==
		static_cast<basic_string_view<CharT, Traits> const&>( rhs );
}



// TODO: This duplicate shouldn't be needed when C++20 is available
template<typename CharT, typename Traits = rftl::char_traits<CharT>, typename Allocator = rftl::allocator<CharT>>
bool operator==(
	basic_string<CharT, Traits, Allocator> const& lhs,
	basic_simple_string<CharT, Traits> const& rhs )
{
	return static_cast<basic_string_view<CharT, Traits> const&>( lhs ) ==
		static_cast<basic_string_view<CharT, Traits> const&>( rhs );
}

///////////////////////////////////////////////////////////////////////////////
}
