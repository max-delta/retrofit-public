#pragma once
#include "project.h"

#include "PPUFwd.h"
#include "PPU/ResourceManager.h"



namespace RF { namespace gfx {
///////////////////////////////////////////////////////////////////////////////

class PPU_API FramePackManager final : public ResourceManager<FramePackBase, ManagedFramePackID, k_InvalidManagedFramePackID>
{
	RF_NO_COPY( FramePackManager );

	//
	// Public methods
public:
	FramePackManager();
	~FramePackManager();


	//
	// Protected methods
protected:
	virtual UniquePtr<ResourceType> AllocateResourceFromFile( Filename const & filename ) override;
};

///////////////////////////////////////////////////////////////////////////////
}}
