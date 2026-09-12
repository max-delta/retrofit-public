#pragma once
#include "HashString.h"

#include "core_math/math_casts.h"

#include "core/meta/FailConsteval.h"


namespace RF::id {
///////////////////////////////////////////////////////////////////////////////

template<typename CharT>
inline constexpr BasicHashString<CharT>::BasicHashString() = default;



template<typename CharT>
template<size_t LenT>
inline consteval RF::id::BasicHashString<CharT>::BasicHashString( const CharT ( &literal )[LenT] )
	: mHash( math::details::ConstStableHashString( literal ) )
	, mView( literal )
{
	if( literal[LenT - 1] != CharT{} )
	{
		FailConsteval( "Expected literal to be null-terminated" );
	}
}



template<typename CharT>
inline constexpr bool BasicHashString<CharT>::IsValid() const
{
	// SUBTLE: If an empty string was set, that's considered valid
	return mView.data() != nullptr;
}



template<typename CharT>
inline constexpr math::HashVal64 BasicHashString<CharT>::GetHash() const
{
	return IsValid() ? mHash : math::details::kFNV_empty_hash;
}



template<typename CharT>
inline constexpr BasicHashString<CharT>::View BasicHashString<CharT>::GetView() const
{
	return mView;
}



template<typename CharT>
inline constexpr bool BasicHashString<CharT>::operator==( BasicHashString const& rhs ) const
{
	if( IsValid() != rhs.IsValid() )
	{
		return false;
	}

	if( GetHash() != rhs.GetHash() )
	{
		return false;
	}

	if( GetView() != rhs.GetView() )
	{
		return false;
	}

	return true;
}

///////////////////////////////////////////////////////////////////////////////
}

template<typename CharT>
inline size_t rftl::hash<RF::id::BasicHashString<CharT>>::operator()( RF::id::BasicHashString<CharT> const& string ) const
{
	RF::math::HashVal64 const hashed = string.GetHash();
	RF_TODO_ANNOTATION( "Hash gets clobbered on 32-bit builds, maybe make a 32-bit sequence hasher?" );
	return RF::math::integer_truncast<size_t>( hashed );
}
