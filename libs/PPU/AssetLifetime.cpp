#include "stdafx.h"
#include "AssetLifetime.h"

#include "PPU/PPUController.h"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

AssetLifetime::AssetLifetime() = default;



AssetLifetime::AssetLifetime(
	WeakPtr<PPUController> ppu,
	DestroyMode mode,
	AssetType type,
	rftl::string_view resourceName )
	: mPpu( ppu )
	, mMode( mode )
	, mType( type )
	, mResourceName( resourceName )
{
	RF_ASSERT( mPpu != nullptr );
	RF_ASSERT( mMode != DestroyMode::Invalid );
	RF_ASSERT( mType != AssetType::Invalid );
	RF_ASSERT( mResourceName.empty() == false );
}



AssetLifetime::AssetLifetime( AssetLifetime&& rhs )
	: RF_MOVE_CONSTRUCT( mPpu )
	, RF_MOVE_CONSTRUCT( mMode )
	, RF_MOVE_CONSTRUCT( mType )
	, RF_MOVE_CONSTRUCT( mResourceName )
{
	RF_MOVE_CLEAR( mPpu );
	RF_MOVE_CLEAR( mMode );
	RF_MOVE_CLEAR( mType );
	RF_MOVE_CLEAR( mResourceName );
}



AssetLifetime::~AssetLifetime()
{
	Clear();
}



AssetLifetime& AssetLifetime::operator=( AssetLifetime&& rhs )
{
	if( this != &rhs )
	{
		Clear();

		RF_MOVE_ASSIGN_CLEAR( mPpu );
		RF_MOVE_ASSIGN_CLEAR( mMode );
		RF_MOVE_ASSIGN_CLEAR( mType );
		RF_MOVE_ASSIGN_CLEAR( mResourceName );
	}
	return *this;
}

///////////////////////////////////////////////////////////////////////////////

void AssetLifetime::Clear()
{
	if( mMode == DestroyMode::Invalid )
	{
		RF_ASSERT( mPpu == nullptr );
		RF_ASSERT( mType == AssetType::Invalid );
		RF_ASSERT( mResourceName.empty() );
		return;
	}

	RF_ASSERT( mPpu != nullptr );
	RF_ASSERT( mType != AssetType::Invalid );
	RF_ASSERT( mResourceName.empty() == false );
	if( mMode == DestroyMode::Deferred )
	{
		bool const success = mPpu->QueueDeferredDestroyRequest( mType, mResourceName );
		RF_ASSERT( success );
	}
	else if( mMode == DestroyMode::Immediate )
	{
		bool const success = mPpu->ForceImmediateDestroyRequest( mType, mResourceName );
		RF_ASSERT( success );
	}
	else
	{
		RF_DBGFAIL();
	}
}

///////////////////////////////////////////////////////////////////////////////
}
