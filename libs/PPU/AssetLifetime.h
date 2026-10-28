#pragma once
#include "project.h"

#include "PPU/PPUFwd.h"

#include "core/ptr/weak_ptr.h"

#include "rftl/string_view"
#include "rftl/string"


namespace RF::gfx::ppu {
///////////////////////////////////////////////////////////////////////////////

// Used to perform automatic destruction when falling out of scope, for putting
//  an RAII-like pattern onto PPU assets
class PPU_API AssetLifetime
{
	RF_NO_COPY( AssetLifetime );

	//
	// Types and constants
public:
	enum class DestroyMode : uint8_t
	{
		Invalid = 0,
		Deferred,
		Immediate
	};


	//
	// Public methods
public:
	AssetLifetime();
	AssetLifetime(
		WeakPtr<PPUController> ppu,
		DestroyMode mode,
		AssetType type,
		rftl::string_view resourceName );
	AssetLifetime( AssetLifetime&& rhs );
	~AssetLifetime();

	AssetLifetime& operator=( AssetLifetime&& rhs );


	//
	// Private methods
private:
	void Clear();


	//
	// Private data
private:
	WeakPtr<PPUController> mPpu = nullptr;
	DestroyMode mMode = DestroyMode::Invalid;
	AssetType mType = AssetType::Invalid;
	rftl::string mResourceName = {};
};

///////////////////////////////////////////////////////////////////////////////
}
