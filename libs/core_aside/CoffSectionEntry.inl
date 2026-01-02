#pragma once
#include "CoffSectionEntry.h"

#include "core/meta/FailConsteval.h"

#include "rftl/extension/algorithms.h"


namespace RF::aside {
///////////////////////////////////////////////////////////////////////////////

inline consteval CoffSectionEntry::CoffSectionEntry( rftl::string_view name )
{
	if( name.size() + sizeof( '\0' ) > mName.size() )
	{
		FailConsteval( "This name is too large to fit in the slot" );
	}
	rftl::copy_overwrite_clamped( mName, name );
	mName.back() = 0;
}



inline consteval CoffSectionEntry::CoffSectionEntry( rftl::string_view name, uint64_t data )
#ifndef __INTELLISENSE__ // Intellisense falls over in MSVC2022
	: CoffSectionEntry( name )
#endif
{
	WriteUInt64( data );
}



inline consteval CoffSectionEntry::CoffSectionEntry( rftl::string_view name, bool data )
#ifndef __INTELLISENSE__ // Intellisense falls over in MSVC2022
	: CoffSectionEntry( name )
#endif
{
	WriteBool( data );
}



inline constexpr rftl::string_view CoffSectionEntry::GetName() const
{
	return mName.data();
}



inline constexpr uint64_t CoffSectionEntry::AsUInt64() const
{
	uint64_t retVal = 0;
	for( size_t i = 0; i < 8; i++ )
	{
		size_t const index = i;

		uint8_t const byte = mData.at( index );
		retVal <<= 8;
		retVal |= byte;
	}
	return retVal;
}



inline constexpr bool CoffSectionEntry::AsBool() const
{
	return AsUInt64() != 0;
}



inline constexpr void CoffSectionEntry::WriteUInt64( uint64_t data )
{
	for( size_t i = 0; i < 8; i++ )
	{
		size_t const index = 7 - i;

		uint8_t const byte = data & 0xFF;
		data >>= 8;

		mData.at( index ) = byte;
	}
}



inline constexpr void CoffSectionEntry::WriteBool( bool data )
{
	WriteUInt64(
		data ?
			0xFFFFFFFFFFFFFFFFull :
			0x0000000000000000ull );
}



inline constexpr CoffSectionEntry CoffSectionEntry::FindEntry( rftl::span<CoffSectionEntry const> entries, rftl::string_view name )
{
	for( CoffSectionEntry const& entry : entries )
	{
		if( entry.GetName() == name )
		{
			return entry;
		}
	}
	return {};
}

///////////////////////////////////////////////////////////////////////////////
}
