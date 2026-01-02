#pragma once

#include "core_aside/AsideFwd.h"

#include "rftl/array"
#include "rftl/span"
#include "rftl/string_view"


namespace RF::aside {
///////////////////////////////////////////////////////////////////////////////

// Intended to be used for small side-channel input / output data, assigned to
//  module sections with easy-to-locate strings to help facilitate external
//  tooling that operates on memory scans
// SEE: Speedrunners using LiveSplit with 'auto splitters' or overlays
// SEE: DFHack-like manipulation tools that would benefit from stability
// EXAMPLE:
//  RF_DECLARE_SECTION_RW( "MySec" );
//  RF_ASSIGN_TO_SECTION( "MySec" )
//  static aside::CoffSectionEntry sMySec[] = {
//    aside::CoffSectionEntry( "MYVALUE", 0x1122334455667788ull ),
//    aside::CoffSectionEntry( "DOSTUFF", true ),
//  };
struct alignas( 16 ) CoffSectionEntry
{
	consteval CoffSectionEntry() = default;
	consteval CoffSectionEntry( rftl::string_view name );
	consteval CoffSectionEntry( rftl::string_view name, uint64_t data );
	consteval CoffSectionEntry( rftl::string_view name, bool data );

	constexpr rftl::string_view GetName() const;

	constexpr uint64_t AsUInt64() const;
	constexpr bool AsBool() const;

	constexpr void WriteUInt64( uint64_t data );
	constexpr void WriteBool( bool data );

	static constexpr CoffSectionEntry FindEntry( rftl::span<CoffSectionEntry const> entries, rftl::string_view name );
	static bool OverwriteEntry( rftl::span<CoffSectionEntry> entries, CoffSectionEntry const& overwrite );


private:
	// Will always be null-terminated
	rftl::array<char, 8> mName = {};

	// Stored big-endian
	rftl::array<uint8_t, 8> mData = {};
};
static_assert( sizeof( CoffSectionEntry ) == 16 );
static_assert( alignof( CoffSectionEntry ) == 16 );

///////////////////////////////////////////////////////////////////////////////
}

#include "CoffSectionEntry.inl"
