#pragma once

#include "core_math/Hash.h"

#include "rftl/string_view"


namespace RF::id {
///////////////////////////////////////////////////////////////////////////////

template<typename CharT>
class BasicHashString
{
	//
	// Types
public:
	using Char = CharT;
	using View = rftl::basic_string_view<Char>;


	//
	// Public methods
public:
	// Default is considered invalid
	constexpr BasicHashString();

	// Under normal conditions, hash strings are only safe when the memory is
	//  guaranteed to not move, which means they need to be literals
	template<size_t LenT>
	consteval BasicHashString( const CharT ( &literal )[LenT] );

	// Returns true if the string was ever affirmatively set, even if set to an
	//  empty string
	constexpr bool IsValid() const;

	// Hash is pre-computed on construction and simply fetched here
	constexpr math::HashVal64 GetHash() const;

	// View will be null if the string was never affirmatively set, otherwise
	//  this will be the string
	constexpr View GetView() const;

	// Tries to avoid comparing character data until it absolutely has to, and
	//  stays in the local cachelines to optimize for the common case where the
	//  comparison would early-out
	constexpr bool operator==( BasicHashString const& rhs ) const;


	//
	// Private data
private:
	math::HashVal64 mHash = 0;
	View mView = {};
};

///////////////////////////////////////////////////////////////////////////////
}

template<typename CharT>
struct rftl::hash<RF::id::BasicHashString<CharT>>
{
	size_t operator()( RF::id::BasicHashString<CharT> const& string ) const;
};

// Explicitly instantiate and alias common types
namespace RF::id {
using HashString = BasicHashString<char>;
using U8HashString = BasicHashString<char8_t>;
using U16HashString = BasicHashString<char16_t>;
using U32HashString = BasicHashString<char32_t>;
}
extern template class RF::id::BasicHashString<char>;
extern template class RF::id::BasicHashString<char8_t>;
extern template class RF::id::BasicHashString<char16_t>;
extern template class RF::id::BasicHashString<char32_t>;

#include "HashString.inl"
